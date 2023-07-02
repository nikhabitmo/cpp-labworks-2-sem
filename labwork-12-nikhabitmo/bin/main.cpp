#include <iostream>
#include <lib/Database.h>
#include <conio.h>

using namespace MyDB;

class output {
 public:
  output() {
    std::cout << output_;
  }
 private:
  std::string output_ = "\n************************************************************************\n";
};

int main() {

  Table table("MyTable");

  std::vector<Table::Value> column1Values;
  column1Values.emplace_back(10);
  column1Values.emplace_back(20);
  column1Values.emplace_back(30);
  table.addColumn("Column1", column1Values);

  std::vector<Table::Value> column2Values;
  column2Values.emplace_back("Hello");
  column2Values.emplace_back("World");
  column2Values.emplace_back("!");
  table.addColumn("Column2", column2Values);

  std::vector<Table::Value> row1Values;
  row1Values.emplace_back(40);
  row1Values.emplace_back("D");
  table.addLine(row1Values);

  std::vector<Table::Value> row2Values;
  row2Values.emplace_back(50);
  row2Values.emplace_back("AAA");
  table.addLine(row2Values);

  std::vector<std::string> columnNames = table.getColumnNames();
  for (const auto& columnName : columnNames) {
    Table::Column column = table.getColumn(columnName);
    std::cout << "Column: " << columnName << ", Is Primary: " << column.is_primary()
              << ", Is Foreign: " << column.is_foreign() << std::endl;

    std::vector<Table::Value> cells = column.cells_;
    for (Table::Value cell : cells) {
      if (cell.index() == 0) {
        std::cout << "  Integer value: " << cell.getValue<int>() << std::endl;
      } else if (cell.index() == 4) {
        std::cout << "  String value: " << cell.getValue<std::string>() << std::endl;
      }
    }
  }

  std::vector<Table::Value> rowValues = table.getLine(2);
  if (!rowValues.empty()) {
    std::cout << "Row 2 values:" << std::endl;
    for (Table::Value value : rowValues) {
      if (value.index() == 0) {
        std::cout << "  Integer value: " << value.getValue<int>() << std::endl;
      } else if (value.index() == 4) {
        std::cout << "  String value: " << value.getValue<std::string>() << std::endl;
      }
    }
  }

  output();
  std::cout << "Creating DB" << std::endl;
  MyCoolDB database("MyInterests");
  std::cout << "Creating Table" << std::endl;
  database.createTable("music");
  output();
  database.addRequest("CREATE TABLE music (SOng vArchar, artist vaRcHar, year inT, )");
  std::cout << "Request: CREATE TABLE music (SOng vArchar, artist vaRcHar, year inT, )" << std::endl;
  database.parse();
  output();
  std::cout << database.parsing_status() << std::endl;
  auto a = database.GetTable("music");
  for (const auto& i : a.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;
  std::cout << a.getColumn("song").cells_.size() << std::endl;
  output();
  std::cout << "Request: SELECT * FROM MUSIC" << std::endl;
  database.addRequest("SELECT * FROM MUSIC");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  auto b = database.selectTable();
  for (const auto& i : b.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();
  std::cout << "Request: SELECT song, artist FROM MUSIC" << std::endl;
  database.addRequest("SELECT song artist FROM MUSIC");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  auto d = database.selectTable();
  for (const auto& i : d.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();
  std::cout << "Request: CREATE TABLE TEMP (column1 varchar, column2 int, column3 bool, )" << std::endl;
  database.addRequest("CREATE TABLE TEMP (column1 varchar, column2 int, column3 bool, )");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  for (const auto& i : database.GetTable("temp").getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();

  std::cout << "Request: DROP TABLE TEMP" << std::endl;
  database.addRequest("DROP TABLE TEMP");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;

  output();

  std::cout << "Trying to get deleted table: temp" << std::endl;
  std::cout << "Request: SELECT * FROM temp" << std::endl;
  database.addRequest("SELECT * FROM temp");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;

  output();
  std::cout << "Trying to delete existing records in a table" << std::endl;
  std::cout << "Request: DELETE FROM music" << std::endl;
  database.addRequest("DELETE FROM music");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  for (const auto& i : database.GetTable("music").getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();

  output();

  std::cout << "Request: SELECT * FROM MUSIC INNER JOIN TEMP ON MUSIC.song = TEMP.column1" << std::endl;
  database.addRequest("SELECT * FROM MUSIC INNER JOIN TEMP ON MUSIC.song = TEMP.column1");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  auto e = database.selectTable();
  for (const auto& i : e.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();

  std::cout << "Request: SELECT * FROM MUSIC RIGHT JOIN TEMP ON MUSIC.song = TEMP.column1" << std::endl;
  database.addRequest("SELECT * FROM MUSIC RIGHT JOIN TEMP ON MUSIC.song = TEMP.column1");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  auto f = database.selectTable();
  for (const auto& i : f.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  output();

  std::cout << "Request: SELECT * FROM MUSIC LEFT JOIN TEMP ON MUSIC.song = TEMP.column1" << std::endl;
  database.addRequest("SELECT * FROM MUSIC LEFT JOIN TEMP ON MUSIC.song = TEMP.column1");
  database.parse();
  std::cout << "Database parsing status: " << database.parsing_status() << std::endl;
  auto g = database.selectTable();
  for (const auto& i : g.getColumnNames()) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;


  MyDB::MyCoolDB database1("Marks");
  database1.addRequest("CREATE TABLE MATHS (column1 int, column2 int, column3 int, column4 int,");
  database1.parse();
  std::vector<Table::Value> row1vals;
  row1vals.emplace_back(0);
  row1vals.emplace_back(0);
  row1vals.emplace_back(0);
  row1vals.emplace_back(0);
  database1.GetTable("maths").addLine(row1vals);
  std::vector<Table::Value> row2vals;
  row2vals.emplace_back(60);
  row2vals.emplace_back(72);
  row2vals.emplace_back(75);
  row2vals.emplace_back(80);
  std::cout << database1.parsing_status() << std::endl;
  database1.GetTable("maths").addLine(row2vals);
  database1.save("../../output.txt");
  output();
  std::cout << "Press ESC to exit\n";
  while (true) {
    if (getch() == 27) {
      return 0;
    }
  }
  return 0;
}
