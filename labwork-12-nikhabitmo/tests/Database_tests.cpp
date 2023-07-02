#include <gtest/gtest.h>
#include <lib/Database.h>

TEST(TEST1, CreateDatabase) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE music (SOng vArchar, artist vaRcHar, year inT, )");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE music (SOng vArchar, artist vaRcHar, year inT, )");
  db.parse();
  ASSERT_FALSE(db.parsing_status());

  db.addRequest("DROP TABLE TEMP");
  db.parse();

  db.addRequest("DROP TABLE music");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST2, UsageDatabase) {
  MyDB::MyCoolDB db("marks");

  db.addRequest("CREATE TABLE marks (maths float, prog_cpp float)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE Table music (SOng vArchar, artist vaRcHar, year inT, )");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST3, InsertDatabase) {
  MyDB::MyCoolDB db("marks");

  db.addRequest("CREATE TABLE marks (maths float, prog_cpp float)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO marks (maths, prog_cpp) VALUES (1.0, 2.0)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO marks (prog_cpp) VALUES (10.9, 2.1)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST4, SelectQuery) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE departments (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO departments (id, name) VALUES (10, 'John')");

  db.addRequest("SELECT * FROM departments");
  db.parse();
  auto a = db.selectTable();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT id, name FROM departments");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
  auto b = db.selectTable();
}

TEST(TEST5, JoinQuery) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE departments (id INT PRIMARY KEY, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT * FROM employees LEFT JOIN departments ON employees.department_id = departments.id");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT * FROM employees INNER JOIN departments ON employees.department_id = departments.id");
  db.parse();

  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST6, PrimaryKey) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE Persons ("
                "    ID int NOT NULL,"
                "    LastName varchar(255) NOT NULL,"
                "    FirstName varchar(255),"
                "    Age int,"
                "    PRIMARY KEY (ID)"
                ");");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE TABLE departments (id INT PRIMARY KEY, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST7, DropTable) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("Create TABLE employees");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("DROP TABLE employees");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("DROP TABLE departments");
  db.parse();
  ASSERT_FALSE(db.parsing_status());
}

TEST(TEST8, UpdateQuery) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("UPDATE employees SET salary = 6000 WHERE department = 'Sales'");
  db.parse();
  ASSERT_FALSE(db.parsing_status());
}

TEST(TEST9, InsertQuery) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (1, 'John Doe', 1)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST10, DeleteQuery) {
  MyDB::MyCoolDB db("test.db");
  db.addRequest("CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT)");
  db.addRequest("DELETE employees");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST11, PrimaryKeyConstraint) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST12, ForeignKeyConstraint) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest(
      "CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT REFERENCES departments(id))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST13, WhereTEST) {
  MyDB::MyCoolDB db("test.db");
  db.addRequest(
      "CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (1, John, 71)");
  db.parse();

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (2, Nikita, 11)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (3, Makar, 90)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT FROM employees where id >= 2");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST14, CreateTableDuplicate) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_FALSE(db.parsing_status());
}

TEST(TEST15, InvalidQuerySyntax) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("SELECT WHERE * FROM employees id = 1");
  db.parse();
  ASSERT_FALSE(db.parsing_status());

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());
}

TEST(TEST16, CreateTableWithConstraints) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT PRIMARY KEY, name VARCHAR(50), department_id INT)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE TABLE departments (id INT PRIMARY KEY, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("CREATE TABLE departments (id INT PRIMARY KEY, name VARCHAR(50), company_id INT)");
  db.parse();
  ASSERT_FALSE(db.parsing_status());
}

TEST(TEST17, UpdateQueryInvalidColumn) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("UPDATE employees SET salary = 6000 WHERE department = 'Sales'");
  db.parse();
  ASSERT_FALSE(db.parsing_status());
}

TEST(TEST18, DeleteQueryNoCondition) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name) VALUES (1, 'John'), (2, 'Jane'), (3, 'Mike')");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("DELETE FROM employees");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT * FROM employees");
  db.parse();
  auto result = db.selectTable();
  ASSERT_TRUE(db.parsing_status());
  ASSERT_EQ(result.getColumns().size(), 0);
}

TEST(TEST19, WhereClause) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50), department_id INT)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (1, 'John', 1)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (2, 'Jane', 2)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name, department_id) VALUES (3, 'Mike', 1)");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT * FROM employees WHERE department_id = 216");
  db.parse();
  auto result = db.selectTable();
  ASSERT_TRUE(db.parsing_status());
  ASSERT_EQ(result.getColumns().size(), 0);
}

TEST(TEST20, InsertIntoQuery) {
  MyDB::MyCoolDB db("test.db");

  db.addRequest("CREATE TABLE employees (id INT, name VARCHAR(50))");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name) VALUES (1, 'John')");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name) VALUES (2, 'Jane')");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("INSERT INTO employees (id, name) VALUES (3, 'Mike')");
  db.parse();
  ASSERT_TRUE(db.parsing_status());

  db.addRequest("SELECT * FROM employees");
  db.parse();
  auto result = db.selectTable();
  ASSERT_TRUE(db.parsing_status());
}
