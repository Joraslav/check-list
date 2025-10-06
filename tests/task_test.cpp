#include "Task.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace task {

// Фикстура для тестов TaskManager
class TaskManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем временную директорию для тестов
        test_dir_ = fs::temp_directory_path() / "task_manager_test";
        fs::create_directories(test_dir_);
        
        // Устанавливаем пути для тестов
        config_dir_ = test_dir_ / "config";
        output_dir_ = test_dir_ / "notepad";
        fs::create_directories(config_dir_);
        fs::create_directories(output_dir_);
        
        config_file_ = config_dir_ / "config_todo.json";
        task_file_ = output_dir_ / "test_list.json";
    }
    
    void TearDown() override {
        // Удаляем временные файлы и директории
        if (fs::exists(test_dir_)) {
            fs::remove_all(test_dir_);
        }
    }
    
    // Вспомогательные методы для создания тестовых файлов
    void CreateConfigFile(const std::string& path, const std::string& name) {
        json config;
        config["path"] = path;
        config["name"] = name;
        
        std::ofstream file(config_file_);
        file << config.dump(4);
        file.close();
    }
    
    void CreateTaskFile(const std::vector<Task>& tasks) {
        json j = json::array();
        for (const auto& task : tasks) {
            j.push_back({{"text", task.text}, {"done", task.done}});
        }
        
        std::ofstream file(task_file_);
        file << j.dump(4);
        file.close();
    }
    
    fs::path test_dir_;
    fs::path config_dir_;
    fs::path output_dir_;
    fs::path config_file_;
    fs::path task_file_;
};

// Тесты для конструктора TaskManager
TEST_F(TaskManagerTest, Constructor_Success) {
    // Подготавливаем тестовые данные
    std::vector<Task> test_tasks = {
        Task("Task 1", false),
        Task("Task 2", true)
    };
    
    CreateConfigFile(output_dir_.string(), "test_list.json");
    CreateTaskFile(test_tasks);
    
    // Тестирование конструктора
    EXPECT_NO_THROW({
        TaskManager manager(config_file_.string());
    });
}

// Тесты для методов AddTask
TEST_F(TaskManagerTest, AddTask_String_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    EXPECT_NO_THROW({
        manager.AddTask("New Task");
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].text, "New Task");
    EXPECT_FALSE(tasks[0].done);
}

TEST_F(TaskManagerTest, AddTask_TaskObject_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    Task new_task("New Task", true);
    EXPECT_NO_THROW({
        manager.AddTask(new_task);
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].text, "New Task");
    EXPECT_TRUE(tasks[0].done);
}

// Тесты для метода ToggleTask
TEST_F(TaskManagerTest, ToggleTask_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    
    // Переключаем статус второй задачи
    EXPECT_NO_THROW({
        manager.ToggleTask(1);
    });
    
    const auto& tasks = manager.GetTasks();
    EXPECT_FALSE(tasks[0].done);
    EXPECT_TRUE(tasks[1].done);
    
    // Переключаем обратно
    EXPECT_NO_THROW({
        manager.ToggleTask(1);
    });
    
    EXPECT_FALSE(tasks[0].done);
    EXPECT_FALSE(tasks[1].done);
}

TEST_F(TaskManagerTest, ToggleTask_InvalidIndex_Throws) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    
    EXPECT_THROW({
        manager.ToggleTask(5);
    }, std::out_of_range);
}

// Тесты для метода RemoveTask
TEST_F(TaskManagerTest, RemoveTask_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    manager.AddTask("Task 3");
    
    EXPECT_NO_THROW({
        manager.RemoveTask(1);
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 2);
    EXPECT_EQ(tasks[0].text, "Task 1");
    EXPECT_EQ(tasks[1].text, "Task 3");
}

TEST_F(TaskManagerTest, RemoveTask_InvalidIndex_Throws) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    
    EXPECT_THROW({
        manager.RemoveTask(5);
    }, std::out_of_range);
}

// Тесты для метода EditTask
TEST_F(TaskManagerTest, EditTask_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    
    EXPECT_NO_THROW({
        manager.EditTask(1, "Updated Task 2");
    });
    
    const auto& tasks = manager.GetTasks();
    EXPECT_EQ(tasks[0].text, "Task 1");
    EXPECT_EQ(tasks[1].text, "Updated Task 2");
}

TEST_F(TaskManagerTest, EditTask_InvalidIndex_Throws) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    
    EXPECT_THROW({
        manager.EditTask(5, "New Task");
    }, std::out_of_range);
}

// Тесты для метода TaskExists
TEST_F(TaskManagerTest, TaskExists_ValidIndex_ReturnsTrue) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    
    EXPECT_TRUE(manager.TaskExists(0));
    EXPECT_TRUE(manager.TaskExists(1));
}

TEST_F(TaskManagerTest, TaskExists_InvalidIndex_ReturnsFalse) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    
    EXPECT_FALSE(manager.TaskExists(5));
    EXPECT_FALSE(manager.TaskExists(1));
}

// Тесты для метода GetTasks
TEST_F(TaskManagerTest, GetTasks_ReturnsCorrectTasks) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    manager.AddTask("Task 3");
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 3);
    EXPECT_EQ(tasks[0].text, "Task 1");
    EXPECT_EQ(tasks[1].text, "Task 2");
    EXPECT_EQ(tasks[2].text, "Task 3");
}

// Тесты для метода Save
TEST_F(TaskManagerTest, Save_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    
    EXPECT_NO_THROW({
        manager.Save();
    });
    
    // Проверяем, что файл был создан и содержит правильные данные
    EXPECT_TRUE(fs::exists(task_file_));
    
    std::ifstream file(task_file_);
    json j = json::parse(file);
    file.close();
    
    ASSERT_TRUE(j.is_array());
    ASSERT_EQ(j.size(), 2);
    EXPECT_EQ(j[0]["text"], "Task 1");
    EXPECT_EQ(j[0]["done"], false);
    EXPECT_EQ(j[1]["text"], "Task 2");
    EXPECT_EQ(j[1]["done"], false);
}

// Тесты для метода LoadTasksFromFile
TEST_F(TaskManagerTest, LoadTasksFromFile_Success) {
    // Подготавливаем тестовые данные
    std::vector<Task> test_tasks = {
        Task("Task 1", false),
        Task("Task 2", true),
        Task("Task 3", false)
    };
    
    CreateConfigFile(output_dir_.string(), "test_list.json");
    CreateTaskFile(test_tasks);
    
    TaskManager manager(config_file_.string());
    EXPECT_NO_THROW({
        manager.LoadTasksFromFile(task_file_.string());
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 3);
    EXPECT_EQ(tasks[0].text, "Task 1");
    EXPECT_EQ(tasks[0].done, false);
    EXPECT_EQ(tasks[1].text, "Task 2");
    EXPECT_EQ(tasks[1].done, true);
    EXPECT_EQ(tasks[2].text, "Task 3");
    EXPECT_EQ(tasks[2].done, false);
}

TEST_F(TaskManagerTest, LoadTasksFromFile_EmptyFile) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    // Создаем пустой файл
    std::ofstream file(task_file_);
    file.close();
    
    TaskManager manager(config_file_.string());
    EXPECT_NO_THROW({
        manager.LoadTasksFromFile(task_file_.string());
    });
    
    const auto& tasks = manager.GetTasks();
    EXPECT_TRUE(tasks.empty());
}

TEST_F(TaskManagerTest, LoadTasksFromFile_NonExistentFile) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    const fs::path non_existent_file = test_dir_ / "non_existent.json";
    
    TaskManager manager(config_file_.string());
    EXPECT_NO_THROW({
        manager.LoadTasksFromFile(non_existent_file.string());
    });
    
    const auto& tasks = manager.GetTasks();
    EXPECT_TRUE(tasks.empty());
}

// Тесты для статических методов SetPath и SetName
TEST_F(TaskManagerTest, SetPath_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    const std::string new_path = (test_dir_ / "new_path").string();
    EXPECT_NO_THROW({
        TaskManager::SetPath(new_path, config_file_.string());
    });
    
    // Проверяем, что значение было записано в конфигурационный файл
    std::ifstream file(config_file_);
    json j = json::parse(file);
    file.close();
    
    EXPECT_EQ(j["path"], new_path);
}

TEST_F(TaskManagerTest, SetName_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    const std::string new_name = "new_list.json";
    EXPECT_NO_THROW({
        TaskManager::SetName(new_name, config_file_.string());
    });
    
    // Проверяем, что значение было записано в конфигурационный файл
    std::ifstream file(config_file_);
    json j = json::parse(file);
    file.close();
    
    EXPECT_EQ(j["name"], new_name);
}

// Тесты для функции MakeDefaultConfig
TEST_F(TaskManagerTest, MakeDefaultConfig_Success) {
    // Удаляем существующие директории, если они есть
    const fs::path default_config_dir = fs::path("../config");
    const fs::path default_output_dir = fs::path("../notepad");
    
    // Проверяем, что директории не существуют до вызова MakeDefaultConfig
    if (fs::exists(default_config_dir)) {
        fs::remove_all(default_config_dir);
    }
    if (fs::exists(default_output_dir)) {
        fs::remove_all(default_output_dir);
    }
    
    EXPECT_NO_THROW({
        MakeDefaultConfig();
    });
    
    // Проверяем, что директории были созданы
    EXPECT_TRUE(fs::exists(default_config_dir));
    EXPECT_TRUE(fs::exists(default_output_dir));
    
    // Проверяем, что конфигурационный файл был создан
    const fs::path config_file = default_config_dir / "config_todo.json";
    EXPECT_TRUE(fs::exists(config_file));
    
    // Проверяем содержимое конфигурационного файла
    std::ifstream file(config_file);
    json j = json::parse(file);
    file.close();
    
    EXPECT_EQ(j["path"], "../notepad");
    EXPECT_EQ(j["name"], "checklist.json");
    
    // Удаляем созданные директории после теста
    if (fs::exists(default_config_dir)) {
        fs::remove_all(default_config_dir);
    }
    if (fs::exists(default_output_dir)) {
        fs::remove_all(default_output_dir);
    }
}

// Тесты для граничных случаев
TEST_F(TaskManagerTest, EmptyTaskList) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    
    // Проверяем, что список задач пуст
    const auto& tasks = manager.GetTasks();
    EXPECT_TRUE(tasks.empty());
    
    // Проверяем, что TaskExists возвращает false для любого индекса
    EXPECT_FALSE(manager.TaskExists(0));
    
    // Проверяем, что ToggleTask, RemoveTask и EditTask бросают исключения для пустого списка
    EXPECT_THROW({
        manager.ToggleTask(0);
    }, std::out_of_range);
    
    EXPECT_THROW({
        manager.RemoveTask(0);
    }, std::out_of_range);
    
    EXPECT_THROW({
        manager.EditTask(0, "New Task");
    }, std::out_of_range);
}

// Дополнительные тесты для граничных случаев и ошибочных ситуаций

// Тесты для работы с пустыми строками
TEST_F(TaskManagerTest, AddTask_EmptyString) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    EXPECT_NO_THROW({
        manager.AddTask("");
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].text, "");
    EXPECT_FALSE(tasks[0].done);
}

// Тесты для работы с очень длинными строками
TEST_F(TaskManagerTest, AddTask_VeryLongString) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    const std::string long_text(10000, 'a'); // Очень длинная строка
    EXPECT_NO_THROW({
        manager.AddTask(long_text);
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].text, long_text);
    EXPECT_FALSE(tasks[0].done);
}

// Тесты для работы с задачами, содержащими специальные символы
TEST_F(TaskManagerTest, AddTask_SpecialCharacters) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    const std::string special_text = "Task with special characters: \n\t\r\"'\\";
    EXPECT_NO_THROW({
        manager.AddTask(special_text);
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].text, special_text);
    EXPECT_FALSE(tasks[0].done);
}

// Тесты для работы с большим количеством задач
TEST_F(TaskManagerTest, AddTask_ManyTasks) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    const size_t num_tasks = 1000;
    
    EXPECT_NO_THROW({
        for (size_t i = 0; i < num_tasks; ++i) {
            manager.AddTask("Task " + std::to_string(i));
        }
    });
    
    const auto& tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), num_tasks);
    
    for (size_t i = 0; i < num_tasks; ++i) {
        EXPECT_EQ(tasks[i].text, "Task " + std::to_string(i));
        EXPECT_FALSE(tasks[i].done);
    }
}

// Тесты для проверки сохранения и загрузки большого количества задач
TEST_F(TaskManagerTest, SaveAndLoad_ManyTasks) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    const size_t num_tasks = 1000;
    
    // Добавляем много задач
    for (size_t i = 0; i < num_tasks; ++i) {
        manager.AddTask("Task " + std::to_string(i));
    }
    
    // Сохраняем задачи
    EXPECT_NO_THROW({
        manager.Save();
    });
    
    // Проверяем, что файл был создан
    EXPECT_TRUE(fs::exists(task_file_));
    
    // Создаем новый менеджер и загружаем задачи
    TaskManager manager2(config_file_.string());
    EXPECT_NO_THROW({
        manager2.LoadTasksFromFile(task_file_.string());
    });
    
    // Проверяем, что задачи загрузились правильно
    const auto& tasks = manager2.GetTasks();
    ASSERT_EQ(tasks.size(), num_tasks);
    
    for (size_t i = 0; i < num_tasks; ++i) {
        EXPECT_EQ(tasks[i].text, "Task " + std::to_string(i));
        EXPECT_FALSE(tasks[i].done);
    }
}

// Тесты для метода ClearTasks
TEST_F(TaskManagerTest, ClearTasks_NonEmptyList_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    manager.AddTask("Task 1");
    manager.AddTask("Task 2");
    manager.AddTask("Task 3");
    
    // Проверяем, что список задач не пуст
    EXPECT_FALSE(manager.GetTasks().empty());
    EXPECT_EQ(manager.GetTasks().size(), 3);
    
    // Очищаем список задач
    EXPECT_NO_THROW({
        manager.ClearTasks();
    });
    
    // Проверяем, что список задач пуст
    EXPECT_TRUE(manager.GetTasks().empty());
    EXPECT_EQ(manager.GetTasks().size(), 0);
}

TEST_F(TaskManagerTest, ClearTasks_EmptyList_Success) {
    CreateConfigFile(output_dir_.string(), "test_list.json");
    
    TaskManager manager(config_file_.string());
    
    // Проверяем, что список задач изначально пуст
    EXPECT_TRUE(manager.GetTasks().empty());
    EXPECT_EQ(manager.GetTasks().size(), 0);
    
    // Очищаем пустой список задач
    EXPECT_NO_THROW({
        manager.ClearTasks();
    });
    
    // Проверяем, что список задач все еще пуст
    EXPECT_TRUE(manager.GetTasks().empty());
    EXPECT_EQ(manager.GetTasks().size(), 0);
}

} // namespace task

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}