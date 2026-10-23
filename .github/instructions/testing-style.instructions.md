---
description: "Use when writing tests, adding test cases or reviewing test files. Covers GTest fixture conventions, naming patterns and assertion rules for this project."
applyTo: "tests/Test_*.cpp"
---

# Стиль написания тестов

- Файлы тестов: `tests/Test_<ClassName>.cpp`
- Класс фикстуры: `Test<ClassName> : public ::testing::Test`
- Именование тестов: `MethodName_Scenario_ExpectedResult`
  ```cpp
  TEST_F(TaskDBTest, AddUser_Throws_For_Duplicate_User_Id) { ... }
  ```
- Константы фикстуры: `static constexpr` с префиксом `k` (`kUserId`, `kDefaultUserName`)
- `ASSERT_*` — для фатальных проверок, `EXPECT_*` — для нефатальных
- Один логический сценарий на один `TEST_F`
- Вспомогательные методы добавлять в класс фикстуры, не дублировать логику в тестах
