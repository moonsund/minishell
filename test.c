static int	is_builtin(const char *name)
{
	if (!name)
		return (0);
	// минимум: cd, echo, pwd, export, unset, env, exit
	// здесь можно сделать сравнение строками
	return (1); // заглушка
}

static int	is_parent_only_builtin(const char *name)
{
	if (!name)
		return (0);
	// cd, export, unset, exit — те, что должны менять состояние shell
	return (1); // заглушка
}

static int	run_builtin_parent(t_shell *sh, t_command *cmd)
{
	(void)sh;
	(void)cmd;
	// важно: применить редиректы В РОДИТЕЛЕ, но потом вернуть STDIN/STDOUT обратно
	// верните status
	return (0);
}

static int	run_builtin_child(t_shell *sh, t_command *cmd)
{
	(void)sh;
	(void)cmd;
	// применить redirs уже сделаны в child до вызова
	// вернуть status, а снаружи exit(status)
	return (0);
}

static void	exec_external_child(t_shell *sh, t_command *cmd, char *const envp[])
{
	(void)sh;
	(void)cmd;
	(void)envp;
	// resolve path + execve
	perror("execve");
	exit(127);
}

int	exec_pipeline(t_shell *sh, const t_pipeline *pl, char *const envp[])
{
	// 1) ФИЛЬТР ДО FORK: (cd/export/unset/exit)
	if (pl && pl->count == 1
		&& pl->cmds[0].argv && pl->cmds[0].argv[0]
		&& is_parent_only_builtin(pl->cmds[0].argv[0]))
	{
        // Что с редиректами для builtin в parent (очень важный нюанс)
        // Если вы выполняете builtin в parent, но команда такая:
        // export A=1 > out.txt
        // то редирект должен сработать. Значит в parent нужно:
        // сохранить текущие stdin/stdout (dup)
        // применить redirs (dup2)
        // выполнить builtin
        // восстановить stdin/stdout обратно
        // Иначе вы “сломаете” shell и все дальнейшие команды будут писать в out.txt.
		return (run_builtin_parent(sh, &pl->cmds[0]));
	}

	// 2) ИНАЧЕ — ваш обычный pipeline-fork loop:
	// while (i < pl->count) { pipe; fork; child: ...; parent: ...; }
	// ВНУТРИ CHILD:
	//   apply_redirs_or_die(cmd);
	//   if (s: builtin -> status = run_builtin_child(...); exit(status);
	//   else exec_external_child(...);

	return (0);
}
