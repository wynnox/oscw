# oscw

### Как запускать

```
cd build
cmake .. && make
```

Компилируется 3 исполняемых файла: storage_program, entrypoint_program, client_program

Запускаем entrypoint_program - программа для взаимодействия с базами данных
если хотим, чтобы база данных хранилась в оперативной памяти, то запускаем с этим флагом
```
 ./entrypoint_program in_memory_cache
```
или так, если хотим, чтобы хранилось в файловой системе
```
./entrypoint_program file_system
```

запуск клиента выглядит так: файл с командами и обязательно http://127.0.0.1:8080, чтобы подключиться к ентрипоинту
```
 ./client_program ../file.txt http://127.0.0.1:8080
```
клиентов можно запускать неограниченное количество 

TODO: сделать рандом слип чтобы прям нормально чекнуть как это робит

Для каждого сервера создается отдельный файл с логами, сервера не повторяются, поэтому данные о том, что куда шло не потеряются

### client

### entrypoint

```c++
void run();
```
Эта функция содержит маршруты для команд пользователей

/command - это маршрут для команд пользователя, связанные с данными 
/add_storage - маршрут для инамическое добавление нового сервера
/remove_storage - маршрут для инамическое удаления сервера

сервер запускается на порту 8080

```c++
std::string get_least_loaded_server_url()std::string get_least_loaded_server_url();
```
Эта функция возвращает URL сервера хранения данных с наименьшей нагрузкой. 
Для этого используется функция `get_least_loaded_server`, которая возвращает индекс сервера с минимальной нагрузкой.

```
std::string send_request_to_storage(const std::string& json_command, const std::string& storage_url, const std::string& method)
```
Функция отправляет запрос на сервер хранения данных

```c++
void start_storage_server(int port);
```
Функция, которая поднимает сервер на новом порту. Для этого используется системный вызов system, который запускает 
исполняемый файл сервера хранения с параметрами порта и режима работы (in_memory_cache или file_system). 
После запуска серверу дается время на инициализацию

TODO: считаю, что всегда есть свободный порт, но надо убедиться что, если будет попытка поднять сервак на порту, на котором 
что-то поднято у меня не повалится все

```c++
void stop_storage_server(const std::string& server_url);
```
Функция останавливает сервер хранения данных по указанному URL. Для этого отправляется POST-запрос на сервер по адресу /shutdown. 
После отправки запроса функция ждет завершения работы сервера.

```c++
crow::response add_storage_server();
```
Функция добавляет новый сервер хранения данных. Она блокирует доступ к списку серверов (с помощью std::lock_guard), 
увеличивает номер порта для нового сервера, запускает сервер через функцию start_storage_server и добавляет его URL в список активных серверов. 
Возвращает ответ с кодом 201 (успешное создание сервера).

```c++
crow::response remove_storage_server();
```
Функция удаляет сервер хранения данных с наименьшей загрузкой. 
Перед удалением она получает все данные с этого сервера, чтобы потом импортировать их на другой сервер. 
Затем сервер останавливается, и данные передаются на сервер с минимальной загрузкой

```c++
int get_server_load(const std::string& server_url);
```
Функция получает текущую нагрузку на сервер по его URL. 
Для этого она отправляет GET-запрос на сервер по адресу /load и возвращает полученное значение нагрузки.

```c++
size_t get_least_loaded_server();
```
Функция находит сервер с наименьшей нагрузкой, перебирая все серверы и запрашивая их текущую нагрузку. 
Возвращает индекс сервера с минимальной нагрузкой.
