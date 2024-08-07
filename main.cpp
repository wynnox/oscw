#include <iostream>
#include <string>
#include <vector>
#include "scheme.h"
#include "pool.h"
#include "collectoin.h"
#include "data.h"

int main() {
    pool data_pool(3);

    scheme student_scheme(3);
    collection group_a(3);
    collection group_b(3);

    group_a.add_data("001", data("John", "Doe"));
    group_a.add_data("002", data("Jane", "Smith"));
    group_b.add_data("003", data("Emily", "Jones"));
    group_b.add_data("004", data("Michael", "Brown"));

    student_scheme.add_collection("Group A", group_a);
    student_scheme.add_collection("Group B", group_b);

    data_pool.add_scheme("Student Scheme", student_scheme);

    std::cout << data_pool << std::endl;

    // Получение данных
    scheme retrieved_scheme = data_pool.get_scheme("Student Scheme");
    collection retrieved_group = retrieved_scheme.get_collection("Group A");
    data student = retrieved_group.get_data("001");

    std::cout << "Retrieved student: " << student.first_name << " " << student.last_name << std::endl;

    return 0;
}