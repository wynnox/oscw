#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>

const size_t PAGE_SIZE = 4096; // Размер страницы в байтах

// Структура страницы
struct Page {
    int nextPageId; // Указатель на следующую страницу (-1 если нет)
    size_t dataSize; // Размер данных на текущей странице
    char data[PAGE_SIZE - sizeof(nextPageId) - sizeof(dataSize)]; // Данные на странице

    Page() : nextPageId(-1), dataSize(0) {
        std::memset(data, 0, sizeof(data));
    }
};

// Структура для хранения данных
struct MyData {
    int key;
    std::string value;

    // Сериализация данных с разбиением на страницы
    void serialize(std::ofstream &outFile, int &currentPageId) const {
        size_t valueLength = value.size();
        const char *valueData = value.c_str();

        while (valueLength > 0) {
            Page page;
            page.nextPageId = -1;

            size_t spaceAvailable = sizeof(page.data);
            size_t bytesToWrite = std::min(spaceAvailable, valueLength);

            // Записываем ключ только на первой странице
            if (currentPageId == 0) {
                outFile.write(reinterpret_cast<const char*>(&key), sizeof(key));
            }

            std::memcpy(page.data, valueData, bytesToWrite);
            page.dataSize = bytesToWrite;

            // Обновляем оставшуюся длину и указатель на данные
            valueLength -= bytesToWrite;
            valueData += bytesToWrite;

            // Сохраняем страницу в файл
            outFile.write(reinterpret_cast<const char*>(&page), sizeof(Page));

            // Если есть еще данные для записи, создаем новую страницу
            if (valueLength > 0) {
                currentPageId++;
                page.nextPageId = currentPageId;
            }
        }
    }

    // Десериализация данных, собранных с нескольких страниц
    void deserialize(std::ifstream &inFile, int startPageId) {
        key = 0;
        std::string tempValue;

        int currentPageId = startPageId;
        do {
            Page page;
            inFile.read(reinterpret_cast<char*>(&page), sizeof(Page));

            // Если это первая страница, считываем ключ
            if (currentPageId == startPageId) {
                inFile.read(reinterpret_cast<char*>(&key), sizeof(key));
            }

            // Добавляем данные страницы в строку
            tempValue.append(page.data, page.dataSize);

            currentPageId = page.nextPageId;
        } while (currentPageId != -1);

        value = tempValue;
    }
};

int main() {
    MyData dataToSave;
    dataToSave.key = 42;
    dataToSave.value = "This is a very long string that might not fit into a single page, so it has to be split across multiple pages.";

    // Сохраняем данные в файл, разбивая их на страницы
    std::ofstream outFile("paged_data.bin", std::ios::binary);
    int pageId = 0;
    dataToSave.serialize(outFile, pageId);
    outFile.close();

    // Загружаем данные из файла, собирая их с нескольких страниц
    MyData dataLoaded;
    std::ifstream inFile("paged_data.bin", std::ios::binary);
    dataLoaded.deserialize(inFile, 0);
    inFile.close();

    // Выводим загруженные данные
    std::cout << "Key: " << dataLoaded.key << std::endl;
    std::cout << "Value: " << dataLoaded.value << std::endl;

    return 0;
}
