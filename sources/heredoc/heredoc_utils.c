#include "minishell.h"

t_exit_status		process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, t_exit_status exit_status);
static t_exit_status get_heredoc(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, size_t *heredoc_index);
static t_exit_status heredoc_loop(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, int fd, char *filename);
static void         redir_replace_with_infile(t_redir *redir, char *filename);
static char         *generate_heredoc_filename(size_t heredoc_index);
static t_exit_status expand_heredoc(char **line, t_env_var_list *env_vars, t_exit_status exit_status);
static t_exit_status write_line_in_fd(int fd, char *line);
static t_exit_status sb_init(t_strbuf *strbuf);
static void          sb_free(t_strbuf *strbuf);
static t_exit_status sb_reserve(t_strbuf *strbuf, size_t extra);
static t_exit_status sb_append_char(t_strbuf *strbuf, char c);
static t_exit_status sb_append_str(t_strbuf *strbuf, const char *s);
static size_t        var_name_end(const char *s, size_t start);
static t_exit_status append_env_value(t_strbuf *strbuf, t_env_var_list *env_vars, const char *s, size_t start, size_t end);
static t_exit_status expand_dollar(t_strbuf *strbuf, const char *src, size_t *i, t_env_var_list *env_vars, t_exit_status exit_status);



t_exit_status process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, t_exit_status last_status)
{
    size_t  i;
    t_redir *redir;
    size_t  heredoc_index;
    t_exit_status status;

    if (!pipeline || !env_vars)
        return (ES_GENERAL);

    heredoc_index = 0;
    i = 0;
    while (i < pipeline->count)
    {
        redir = pipeline->cmds[i].redirs;
        while (redir)
        {
            if (redir->type == R_HEREDOC)
            {
                status = get_heredoc(redir, env_vars, last_status, &heredoc_index);
                if (status != ES_SUCCESS)
                    return (status);
            }
            redir = redir->next;
        }
        i++;
    }
    return (ES_SUCCESS);
}

static t_exit_status get_heredoc(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, size_t *heredoc_index)
{
    char        *heredoc_filename;
    int         fd;
    t_exit_status st;

    heredoc_filename = generate_heredoc_filename((*heredoc_index)++);
    if (!heredoc_filename)
        return (ES_GENERAL);

    fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0)
    {
        free(heredoc_filename);
        return (ES_GENERAL);
    }

    st = heredoc_loop(redir, env_vars, exit_status, fd, heredoc_filename);
    if (st != ES_SUCCESS)
        return (st);

    close(fd);
    redir_replace_with_infile(redir, heredoc_filename);
    return (ES_SUCCESS);
}

/* One place to do failure cleanup */
static t_exit_status heredoc_cleanup_return(int fd, char *filename, t_exit_status st)
{
    if (fd >= 0)
        close(fd);
    if (filename)
    {
        unlink(filename);
        free(filename);
    }
    return (st);
}

static t_exit_status heredoc_loop(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, int fd, char *filename)
{
    char        *line;
    t_exit_status status;

    while (true)
    {
        line = readline("heredoc> ");

        if (g_sigint) /* Ctrl+C */
        {
            g_sigint = 0;
            return (heredoc_cleanup_return(fd, filename, ES_SIGINT));
        }

        if (!line) /* Ctrl+D (EOF) */
            break;

        if (redir->target && ft_strcmp(line, redir->target) == 0)
        {
            free(line);
            break;
        }

        if (redir->expand)
        {
            status = expand_heredoc(&line, env_vars, exit_status);
            if (status != ES_SUCCESS)
            {
                free(line);
                return (heredoc_cleanup_return(fd, filename, status));
            }
        }

        status = write_line_in_fd(fd, line);
        if (status != ES_SUCCESS)
        {
            free(line);
            return (heredoc_cleanup_return(fd, filename, status));
        }

        free(line);
    }

    return (ES_SUCCESS);
}

static t_exit_status sb_init(t_strbuf *strbuf)
{
    strbuf->buf = (char *)malloc(1);
    if (!strbuf->buf)
        return (ES_GENERAL);
    strbuf->buf[0] = '\0';
    strbuf->len = 0;
    strbuf->cap = 1;
    return (ES_SUCCESS);
}

static void sb_free(t_strbuf *strbuf)
{
    if (strbuf && strbuf->buf)
        free(strbuf->buf);
    if (strbuf)
    {
        strbuf->buf = NULL;
        strbuf->len = 0;
        strbuf->cap = 0;
    }
}

static t_exit_status sb_reserve(t_strbuf *strbuf, size_t extra)
{
    char    *tmp;
    size_t  need;
    size_t  new_cap;

    need = strbuf->len + extra + 1;
    if (need <= strbuf->cap)
        return (ES_SUCCESS);

    new_cap = strbuf->cap;
    while (new_cap < need)
        new_cap *= 2;

    tmp = (char *)malloc(new_cap);
    if (!tmp)
        return (ES_GENERAL);

    ft_memcpy(tmp, strbuf->buf, strbuf->len + 1);
    free(strbuf->buf);
    strbuf->buf = tmp;
    strbuf->cap = new_cap;
    return (ES_SUCCESS);
}

static t_exit_status sb_append_char(t_strbuf *strbuf, char c)
{
    t_exit_status st;

    st = sb_reserve(strbuf, 1);
    if (st != ES_SUCCESS)
        return (st);

    strbuf->buf[strbuf->len] = c;
    strbuf->len += 1;
    strbuf->buf[strbuf->len] = '\0';
    return (ES_SUCCESS);
}

static t_exit_status sb_append_str(t_strbuf *strbuf, const char *s)
{
    size_t        n;
    t_exit_status st;

    if (!s || s[0] == '\0')
        return (ES_SUCCESS);

    n = ft_strlen(s);
    st = sb_reserve(strbuf, n);
    if (st != ES_SUCCESS)
        return (st);

    ft_memcpy(strbuf->buf + strbuf->len, s, n);
    strbuf->len += n;
    strbuf->buf[strbuf->len] = '\0';
    return (ES_SUCCESS);
}

static size_t var_name_end(const char *str, size_t start)
{
    size_t j;

    j = start;
    while (str[j] && (ft_isalnum((unsigned char)str[j]) || str[j] == '_'))
        j++;
    return (j);
}

static t_exit_status append_env_value(t_strbuf *strbuf, t_env_var_list *env_vars, const char *s, size_t start, size_t end)
{
    char    *name;
    char    *val;

    name = (char *)malloc((end - start) + 1);
    if (!name)
        return (ES_GENERAL);

    ft_memcpy(name, s + start, end - start);
    name[end - start] = '\0';

    val = get_var_value(env_vars, name);
    free(name);

    if (val && val[0] != '\0')
        return (sb_append_str(strbuf, val));
    return (ES_SUCCESS);
}

static t_exit_status expand_dollar(t_strbuf *strbuf, const char *src, size_t *i, t_env_var_list *env_vars, t_exit_status exit_status)
{
    size_t        start;
    size_t        end;
    t_exit_status status;

    if (!src[*i + 1])
    {
        status = sb_append_char(strbuf, '$');
        if (status != ES_SUCCESS)
            return (status);
        *i += 1;
        return (ES_SUCCESS);
    }
    if (src[*i + 1] == '?')
    {
        status = sb_append_str(strbuf, get_last_status_string(exit_status));
        if (status != ES_SUCCESS)
            return (status);
        *i += 2;
        return (ES_SUCCESS);
    }

    start = *i + 1;
    end = var_name_end(src, start);
    if (end == start)
    {
        status = sb_append_char(strbuf, '$');
        if (status != ES_SUCCESS)
            return (status);
        *i += 1;
        return (ES_SUCCESS);
    }

    status = append_env_value(strbuf, env_vars, src, start, end);
    if (status != ES_SUCCESS)
        return (status);

    *i = end;
    return (ES_SUCCESS);
}

static t_exit_status expand_heredoc(char **line, t_env_var_list *env_vars, t_exit_status exit_status)
{
    t_strbuf         strbuf;
    const char   *src;
    size_t       i;
    t_exit_status status;

    if (!line || !*line || !env_vars)
        return (ES_GENERAL);

    status = sb_init(&strbuf);
    if (status != ES_SUCCESS)
        return (status);

    src = *line;
    i = 0;
    while (src[i])
    {
        if (src[i] == '$')
            status = expand_dollar(&strbuf, src, &i, env_vars, exit_status);
        else
        {
            status = sb_append_char(&strbuf, src[i]);
            i++;
        }
        if (status != ES_SUCCESS)
        {
            sb_free(&strbuf);
            return (status);
        }
    }

    free(*line);
    *line = strbuf.buf;
    return (ES_SUCCESS);
}

static void redir_replace_with_infile(t_redir *r, char *filename)
{
    free(r->target);
    r->target = filename;

    r->type = R_IN;
    r->fd = 0;
    r->expand = 0;
}

static char *generate_heredoc_filename(size_t heredoc_index)
{
    char *file_index;
    char *file_name;

    file_index = ft_itoa(heredoc_index);
    if (!file_index)
        return (NULL);
    file_name = ft_strjoin(".heredoc_", file_index);
    free(file_index);
    return (file_name);
}

static t_exit_status write_line_in_fd(int fd, char *line)
{
    ssize_t written;
    size_t  len;
    size_t  off;

    if (!line)
        line = "";

    len = ft_strlen(line);
    off = 0;
    while (off < len)
    {
        written = write(fd, line + off, len - off);
        if (written < 0)
            return (0);
        off += written;
    }

    written = write(fd, "\n", 1);
    if (written < 0)
        return (0);

    return (1);
}
