#include "minishell.h"

int process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars);
static char *generate_heredoc_filename(size_t heredoc_index);
static int expand_heredoc(char **line, t_env_var_list *env_vars);
static int write_heredoc_line(int fd, char *line);
static int append_charter(char **line, char c);
static int append_string(char **line, const char *str);

int process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars)
{
    size_t  i;
    char    *line;
    int  fd;
    char *heredoc_filename;
    size_t heredoc_index;

    heredoc_index = 0;

    if (!pipeline || !env_vars)
        return (0);

    i = 0;
    while (i < pipeline->count)
    {
        if (pipeline->cmds[i].has_heredoc)
        {
            heredoc_filename = generate_heredoc_filename(heredoc_index++);
            if (!heredoc_filename)
                return (0);
            
            fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (fd < 0)
            {
                free(heredoc_filename);
                return (0);
            }

            while (true)
            {
                line = readline("heredoc> ");
                if (!line)
                {
                    free(heredoc_filename);
                    close(fd);
                    return (0);
                }
                    
                if (pipeline->cmds[i].heredoc_limiter
                    && ft_strcmp(line, pipeline->cmds[i].heredoc_limiter) == 0)
                {
                    free(line);
                    break;
                }

                if (!is_empty(line))
                    add_history(line);

                if (pipeline->cmds[i].heredoc_expand_needed)
                {
                    if (!expand_heredoc(&line, env_vars))
                    {
                        free(line);
                        close(fd);
                        free(heredoc_filename);
                        return (0);
                    }
                }

                if (!write_heredoc_line(fd, line))
                {
                    free(line);
                    close(fd);
                    free(heredoc_filename);
                    return (0);
                }

                free(line);
            }
            close(fd);
            pipeline->cmds[i].infile = heredoc_filename;
        }
        i++;
    }
    return (1);
}

static char *generate_heredoc_filename(size_t heredoc_index)
{
    char *file_index;
    char *file_name;

    file_index = ft_itoa(heredoc_index);
    if (!file_index)
        return (NULL);
    file_name = ft_strjoin(".heredoc_", file_index);
    free (file_index);
    return (file_name);
}

static int expand_heredoc(char **line, t_env_var_list *env_vars)
{
    size_t  i;
    size_t  j;
    size_t  start;
    char    *new_line;
    char    *var_name;
    size_t  var_length;
    char    *val;

    if (!line)
        return (0);

    new_line = NULL;
    i = 0;

    while ((*line)[i])
    {
        if ((*line)[i] == '$')
        {
            if (!(*line)[i + 1])
            {
                if (!append_charter(&new_line, (*line)[i]))
                {
                    free(new_line);
                    return (0);
                }
                i++;
                continue;
            }

            else if ((*line)[i + 1] == '?')
            {
                if (!append_string(&new_line, get_last_status_string()))
                {
                    free(new_line);
                    return (0);
                }
                i += 2;
                continue;
            }

            start = i + 1;
            j = start;
            while ((*line)[j]
                && (ft_isalnum((unsigned char)(*line)[j]) || (*line)[j] == '_'))
                j++;

            var_length = j - start;

            if (var_length == 0)
            {
                if (!append_charter(&new_line, (*line)[i]))
                {
                    free(new_line);
                    return (0);
                }
                i++;
                continue;
            }

            var_name = malloc(var_length + 1);
            if (!var_name)
            {
                free(new_line);
                return (0);
            }

            ft_memcpy(var_name, *line + start, var_length);
            var_name[var_length] = '\0';

            val = get_var_value(env_vars, var_name);
            free(var_name);

            if (val && val[0] != '\0')
            {
                if (!append_string(&new_line, val))
                {
                    free(new_line);
                    return (0);
                }
            }
            i = j;
            continue;
        }
        else
        {
            if (!append_charter(&new_line, (*line)[i]))
            {
                free(new_line);
                return (0);
            }
        }
        i++;
    }
    free(*line);
    *line = new_line;
    return (1);
}

static int write_heredoc_line(int fd, char *line)
{
    ssize_t bytes_written;
    size_t line_length;

    if (!line)
        return (1);
    
    line_length = ft_strlen(line);
    if (line_length > 0)
    {
        bytes_written = write(fd, line, line_length);
        if (bytes_written < 0)
            return (0);
    }
    bytes_written = write(fd, "\n", 1);
    if (bytes_written < 0)
            return (0);
    return (1);
}

static int append_charter(char **line, char c)
{
    char *new_line;
    size_t line_length;

    if (!line)
        return (0);
    
    if (!*line)
    {    
        new_line = malloc(2);
        if (!new_line)
            return (0);
        new_line[0] = c;
        new_line[1] = '\0';
        *line = new_line;
        return (1);
    }
    else
    {
        line_length = ft_strlen(*line);
        new_line = malloc(line_length + 2);
        if (!new_line)
            return (0);
        ft_memcpy(new_line, *line, line_length);
        new_line[line_length] = c;
        new_line[line_length + 1] = '\0';
        free(*line);
        *line = new_line;
        return (1);
    }
}

static int append_string(char **line, const char *str)
{
    size_t line_length;
    size_t str_length;
    char *new_line;

    if (!line)
        return (0);

    str_length = ft_strlen(str);

    if (!*line)
    {   
        new_line = malloc(str_length + 1);
        if (!new_line)
            return (0);
        ft_memcpy(new_line, str, str_length);
        new_line[str_length] = '\0';
        *line = new_line;
        return (1);
    }
    else
    {
        line_length = ft_strlen(*line);
        new_line = malloc(str_length + line_length + 1);
        if (!new_line)
            return (0);
        ft_memcpy(new_line, *line, line_length);
        ft_memcpy(new_line + line_length, str, str_length);
        new_line[str_length + line_length] = '\0';
        
        free(line);
        *line = new_line;
        return (1);
    }
}
