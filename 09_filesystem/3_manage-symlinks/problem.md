# posix/filesystem/manage-symlinks

Программе на стандартном потоке ввода передаётся список имён файлов.

Если файл является символической ссылкой, то нужно вывести абсолютное имя того файла, на который эта ссылка указывает.

Если файл является регулярным, то необходимо создать символическую ссылку в текущем каталоге, приписав в названии файла
префикс link_to_.