# fuse/unzipfs

Реализуйте файловую систему, доступную только для чтения, которая обеспечивает доступ к содержимому ZIP-архива.

Внутри файловой системы могут быть только регулярные файлы с правами 0444 и подкаталоги с правами 0555.

Программа для реализации файловой системы должна поддерживать стандартный набор опций FUSE и опцию для указания имени
файла с образом файловой системы --src ИМЯ_ФАЙЛА.zip.

Используйте библиотеку FUSE версии 3.0 и выше, и библиотеку libzip. На сервер нужно отпарвить только исходный файл,
который будет скомпилирован и слинкован с нужными опциями.
