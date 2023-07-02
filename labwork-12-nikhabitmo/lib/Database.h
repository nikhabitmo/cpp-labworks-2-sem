#pragma once

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <variant>

struct COMMAND {
  std::string CREATE{"create"};
  std::string TABLE{"table"};
  std::string SELECT{"select"};
  std::string AS{"as"};
  std::string FROM{"from"};
  std::string WHERE{"where"};
  std::string DROP{"drop"};
  std::string DATABASE{"database"};
  std::string DELETE{"delete"};
  std::string INNER{"inner"};
  std::string LEFT{"left"};
  std::string RIGHT{"right"};
  std::string JOIN{"join"};
  std::string INTO{"into"};
  std::string ON{"on"};
  std::string UPDATE{"update"};
  std::string VALUES{"values"};
  std::string INSERT{"insert"};
  std::string PRIMARY{"primary"};
  std::string FOREIGN{"foreign"};
  std::string NOT{"not"};
  std::string AND{"and"};
  std::string OR{"or"};
  std::string KEY{"key"};
};

struct DATATYPE {
  std::string INT{"int"};
  std::string FLOAT{"float"};
  std::string DOUBLE{"double"};
  std::string BOOL{"bool"};
  std::string VARCHAR{"varchar"};
  std::string NULl{"null"};
};

struct OPERAND {
  bool OR(bool a, bool b) {
    return a || b;
  };

  bool AND(bool a, bool b) {
    return a && b;
  };

  bool NOT(bool a) {
    return !a;
  };
};

namespace MyDB {
COMMAND COMMANDS;
DATATYPE DATATYPES;
OPERAND OPERANDS;

struct SyntaxErrors {
  std::map<std::string, std::string> description;
  SyntaxErrors() {
    description.insert(std::pair<std::string, std::string>(COMMANDS.CREATE,
                                                           "Invalid syntax. Expected: CREATE TABLE <table_name> (<column_name_1> <data_type_i>, <column_name_2> <data_type_i>,...) or CREATE TABLE <table_name> AS SELECT <column_name_1>, <column_name2>,... FROM <existing_table_name>"));
    description.insert(std::pair<std::string, std::string>(COMMANDS.FROM,
                                                           "CREATE TABLE <table_name> (<column_name> <datatype>)"));
    description.insert(std::pair<std::string, std::string>(COMMANDS.SELECT, "SELECT <column_name> FROM <table_name>"));
    description.insert(std::pair<std::string, std::string>(COMMANDS.AS,
                                                           "SELECT <column_name> FROM <table_name> AS <new_table_name>"));
    description.insert(std::pair<std::string, std::string>(COMMANDS.ON,
                                                           "Invalid syntax. Expected: SELECT column_name(s)\n"
                                                           "FROM table1"
                                                           "INNER JOIN table2"
                                                           "ON table1.column_name = table2.column_name;"));
  }
};

SyntaxErrors syntax_error;

class FileManager {
 public:

  FileManager() {};

  explicit FileManager(const std::filesystem::path& path) {
    try {
      output_file_.open(path, std::ios::binary | std::ios::out);
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }

  ~FileManager() {
    output_file_.close();
  }

  std::ofstream output_file_{};
};

class Table {
 public:

  Table() = default;

  virtual ~Table() = default;

  [[nodiscard]] const std::string& getName() const {
    return name_;
  }

  class Value {
   public:
    Value()
        : value_(std::monostate()) {}

    template<class T>
    explicit Value(const T& value)
        : value_(value) {};

    template<class T>
    void push(const T& value) {
      value_.emplace<T>(value);
    }

    template<class T>
    const T& getValue() const {
      return std::get<T>(value_);
    }

    [[nodiscard]] size_t index() const {
      return value_.index();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Value& value) {
      ostream << value;
      return ostream;
    }

    bool operator==(const Value& other) const {
      return value_ == other.value_;
    }

    bool operator!=(const Value& other) const {
      return !(value_ == other.value_);
    }

    bool operator>(const Value& other) const {
      return value_ > other.value_;
    }

    bool operator<(const Value& other) const {
      return value_ < other.value_;
    }

    bool operator<=(const Value& other) const {
      return !(value_ > other.value_);
    }

    bool operator>=(const Value& other) const {
      return !(value_ < other.value_);
    }

   private:
    std::variant<int, float, double, bool, std::string, std::monostate> value_;
  };

  class Column {
   public:
    Column()
        : is_primary_(false), is_foreign_(false) {};

    void addType(const std::string& type) {
      type_ = type;
    }

    const std::string& getType() const {
      return type_;
    }

    explicit Column(const std::vector<Value>& vector)
        : cells_(vector) {
      if (vector.empty()) {
        type_ = DATATYPES.NULl;
      } else {
        if (vector[0].index() == 0) {
          type_ = DATATYPES.INT;
        } else if (vector[0].index() == 1) {
          type_ = DATATYPES.FLOAT;
        } else if (vector[0].index() == 2) {
          type_ = DATATYPES.DOUBLE;
        } else if (vector[0].index() == 3) {
          type_ = DATATYPES.BOOL;
        } else if (vector[0].index() == 4) {
          type_ = DATATYPES.VARCHAR;
        } else if (vector[0].index() == 5) {
          type_ = DATATYPES.NULl;
        }
      }
    };

    void make_primary() {
      is_primary_ = true;
    }

    void make_foreign() {
      is_foreign_ = true;
    }

    [[nodiscard]] bool is_primary() const {
      return is_primary_;
    }

    [[nodiscard]] bool is_foreign() const {
      return is_foreign_;
    }

    std::vector<Value> cells_;

   private:
    std::string type_;
    bool is_primary_{false};
    bool is_foreign_{false};
  };

  explicit Table(const std::string& name)
      : name_(name) {};

  Table& operator=(const Table&) = default;

  void createColumn(const std::string& column_name, const std::string& column_type) {
    if (columns_.count(column_name) > 0) {
      std::cerr << "Column '" << column_name << "' already exists in the table." << std::endl;
      return;
    }
    Column column;
    column.addType(column_type);
    columns_[column_name] = column;
    all_column_names_.push_back(column_name);
  }

  void addColumn(const std::string& column_name, const std::vector<Value>& values) {
    if (columns_.count(column_name) > 0) {
      std::cerr << "Column '" << column_name << "' already exists in the table." << std::endl;
      return;
    }
    Column column(values);
    columns_[column_name] = column;
    all_column_names_.push_back(column_name);
  }

  [[nodiscard]] const std::vector<std::string>& getColumnNames() const {
    return all_column_names_;
  }

  [[nodiscard]] Column& getColumn(const std::string& columnName) const {
    if (columns_.count(columnName) > 0) {
      return const_cast<Column&>(columns_.at(columnName));
    } else {
      std::cerr << "Column '" << columnName << "' does not exist in the table." << std::endl;
    }
    static Column defaultColumn;
    return defaultColumn;
  }

  void deleteColumns() {
    auto columns = getColumns();
    for (auto& column : columns) {
      for (auto& cell : column.cells_) {
        cell = Value();
      }
    }
  }

  void addLine(const std::vector<Value>& line) {
    if (line.size() != all_column_names_.size()) {
      std::cerr << "Row size does not match the number of columns." << std::endl;
      return;
    }
    for (size_t i = 0; i < all_column_names_.size(); i++) {
      const std::string& column_name = all_column_names_[i];
      if (columns_.count(column_name) > 0) {
        columns_[column_name].cells_.push_back(line[i]);
      } else {
        std::cerr << "Column '" << column_name << "' does not exist in the table." << std::endl;
      }
    }
  }

  [[nodiscard]] std::vector<Value> getLine(size_t index) const {
    std::vector<Value> line;
    for (const auto& column : columns_) {
      const std::vector<Value>& columnCells = column.second.cells_;
      if (index < columnCells.size()) {
        line.push_back(columnCells[index]);
      } else {
        std::cerr << "Row index out of range." << std::endl;
        return {};
      }
    }
    return line;
  }

  [[nodiscard]] std::vector<Column> getColumns() const {
    std::vector<Column> result;
    for (const auto& column_name : all_column_names_) {
      result.push_back(getColumn(column_name));
    }
    return result;
  }

  [[nodiscard]] bool isColumnExists(const std::string& column_name) const {
    return columns_.count(column_name) > 0;
  }

 private:
  std::string name_{};
  std::map<std::string, Column> columns_;
  std::vector<std::string> all_column_names_;
};

class ParserResponsesSQL {
 public:

  ParserResponsesSQL() {

  }

  explicit ParserResponsesSQL(const std::string& request)
      : request_(request) {
    std::transform(request_.begin(), request_.end(), request_.begin(), tolower);
    input_commands_ = split(request_);
  }

  void addRequest(const std::string& request) {
    request_ = request;
    std::transform(request_.begin(), request_.end(), request_.begin(), tolower);
    input_commands_ = split(request_);
  }

  friend std::ostream& operator<<(std::ostream& ostream, const ParserResponsesSQL& parser_responses_sql) {
    ostream << parser_responses_sql.request_ << '\n' << parser_responses_sql.input_commands_[0] << ' '
            << parser_responses_sql.input_commands_[1] << ' ' << parser_responses_sql.input_commands_[2] << ' '
            << parser_responses_sql.input_commands_[3];
    return ostream;
  }

  void change_parsing_status(bool status) {
    parsing_status_ = status;
  }

  [[nodiscard]] bool parsing_status() const {
    return parsing_status_;
  }

  [[nodiscard]] const std::vector<std::string>& input_commands() const {
    return input_commands_;
  }

  [[nodiscard]] const std::string& input_commands(size_t index) const {
    return input_commands_[index];
  }

  [[nodiscard]] const std::string& request() const {
    return request_;
  }

  [[nodiscard]] Table& selectTable() {
    return selected_table_;
  }

 private:
  std::string request_;
  std::vector<std::string> input_commands_;
  bool parsing_status_ = false;
  Table selected_table_;

  static std::vector<std::string> split(const std::string& request) {
    std::vector<std::string> result;
    std::string current;
    for (char c : request) {
      if (std::isspace(c)) {
        if (!current.empty()) {
          result.push_back(current);
          current.clear();
        }
      } else if (std::isalnum(c) || c == '*' || c == '>' || c == '<' || c == '=') {
        current += c;
      }
    }
    if (!current.empty()) {
      result.push_back(current);
    }
    return result;
  }
};

class MyCoolDB {
 public:
  explicit MyCoolDB(const std::string& name)
      : name_(name) {};

  MyCoolDB& operator=(const MyCoolDB&) = default;

  ~MyCoolDB() = default;

  void createTable(const std::string& name) {
    if (table_.count(name) > 0) {
      std::cerr << "Table '" << name << "' already exists." << std::endl;
      return;
    }
    table_[name] = Table(name);
    all_tables_.push_back(name);
  };

  void addRequest(const std::string& request) {
    parser_responses_.addRequest(request);
  }

  [[nodiscard]] bool parsing_status() const {
    return parser_responses_.parsing_status();
  }

  Table& GetTable(const std::string& name) {
    try {
      return table_.at(name);
    } catch (const std::out_of_range& oor) {
      std::cerr << "Table '" << name << "' does not exist." << std::endl;
    }
  }

  Table& selectTable() {
    return selected_table_;
  }

  void save(const std::string& path) {
    std::ofstream output_file(path, std::ios::binary | std::ios::out);
    if (!output_file.is_open()) {
      std::cerr << "Couln't save to  the file: " << path << std::endl;
      return;
    }
    output_file << name_ << "||\n";  // DB_NAME||
    for (const auto&
          id : all_tables_) {                          // TABLE_NAME|COLUMNS_SIZE|LINES_SIZE|COLUMN_NAME1-type: cell1, cell2, cell3|COLUMN_NAME2...)
      const auto& table = table_[id];
      output_file << table.getName().c_str() << '|';
      output_file << table.getColumnNames().size() << '|'
                  << table.getColumn(table.getColumnNames()[0]).cells_.size() << '\n';

      for (auto i = 0; i < table.getColumnNames().size(); ++i) {
        output_file << table.getColumnNames()[i].c_str() << '-';
        if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.INT) {
          output_file << DATATYPES.INT << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            if (cell.index() == 5) {
              output_file << DATATYPES.NULl << ',';
            } else {
              output_file << cell.getValue<int>() << ',';
            }
          }
        } else if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.FLOAT) {
          output_file << DATATYPES.FLOAT << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            if (cell.index() == 5) {
              output_file << DATATYPES.NULl << ',';
            } else {
              output_file << cell.getValue<float>() << ',';
            }
          }
        } else if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.DOUBLE) {
          output_file << DATATYPES.DOUBLE << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            if (cell.index() == 5) {
              output_file << DATATYPES.NULl << ',';
            } else {
              output_file << cell.getValue<double>() << ',';
            }
          }
        } else if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.BOOL) {
          output_file << DATATYPES.BOOL << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            if (cell.index() == 5) {
              output_file << DATATYPES.NULl << ',';
            } else {
              output_file << cell.getValue<bool>() << ',';
            }
          }
        } else if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.VARCHAR) {
          output_file << DATATYPES.VARCHAR << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            if (cell.index() == 5) {
              output_file << DATATYPES.NULl << ',';
            } else {
              output_file << cell.getValue<std::string>() << ',';
            }
          }
        } else if (table.getColumn(table.getColumnNames()[i]).getType() == DATATYPES.NULl) {
          output_file << DATATYPES.NULl << ':';
          for (Table::Value& cell : table.getColumn(table.getColumnNames()[i]).cells_) {
            output_file << DATATYPES.NULl << ',';
          }
        }
        output_file << '|';
      }
    }
    output_file.close();
  }

  void parse() {
    parser_responses_.change_parsing_status(true);
    for (size_t i = 0; i < parser_responses_.input_commands().size(); ++i) {

      if (parser_responses_.input_commands(i) == COMMANDS.CREATE) { // CREATE TABLE name (a type, b type, c type,...)

        if (i + 1 < parser_responses_.input_commands().size()
            && parser_responses_.input_commands(i + 1) == COMMANDS.TABLE
            && parser_responses_.input_commands(i + 3) != COMMANDS.AS) {
          if (table_.count(parser_responses_.input_commands(i+2)) > 0) {
            parser_responses_.change_parsing_status(false);
            std::cerr << "Table " << parser_responses_.input_commands(3) << "already exists" << std::endl;
            return;
          }
          std::string table_name = parser_responses_.input_commands(i + 2);
          Table new_table(table_name);
          for (size_t j = i + 3; j < parser_responses_.input_commands().size() - 1; j += 2) {
            new_table.createColumn(parser_responses_.input_commands(j), parser_responses_.input_commands(j + 1));
          }
          table_[table_name] = new_table;
          all_tables_.push_back(table_name);

        } else if (parser_responses_.input_commands(i + 1) == COMMANDS.TABLE && parser_responses_.input_commands(i + 3)
            == COMMANDS.AS) { // CREATE TABLE name AS SELECT column1, column2,... FROM existing_table_name
          std::string table_name = parser_responses_.input_commands(i + 2);
          Table new_table(table_name);
          if (parser_responses_.input_commands(i + 3) == COMMANDS.SELECT) {
            auto index_FROM =
                std::find(parser_responses_.input_commands().begin(),
                          parser_responses_.input_commands().end(),
                          COMMANDS.FROM);
            if (index_FROM != parser_responses_.input_commands().end()
                && parser_responses_.input_commands().size() - i - 2 - 1 > 0) {
              auto index_existing_table = index_FROM + 1;
              if (table_.count(*index_existing_table) > 0) {
                auto index_WHERE = std::find(parser_responses_.input_commands().begin(),
                                             parser_responses_.input_commands().end(),
                                             COMMANDS.WHERE);
                if (index_WHERE != parser_responses_.input_commands().end()) {
                  auto indexMORE = std::find(parser_responses_.input_commands().begin(),
                                             parser_responses_.input_commands().end(),
                                             ">");
                  auto indexLESS = std::find(parser_responses_.input_commands().begin(),
                                             parser_responses_.input_commands().end(),
                                             "<");
                  auto indexEQ = std::find(parser_responses_.input_commands().begin(),
                                           parser_responses_.input_commands().end(),
                                           "=");
                  auto indexMOREEQ = std::find(parser_responses_.input_commands().begin(),
                                             parser_responses_.input_commands().end(),
                                             ">=");
                  auto indexLESSEQ = std::find(parser_responses_.input_commands().begin(),
                                             parser_responses_.input_commands().end(),
                                             "<=");
                  auto indexNOTEQ = std::find(parser_responses_.input_commands().begin(),
                                           parser_responses_.input_commands().end(),
                                           "<>");
                  auto indexNOT = std::find(parser_responses_.input_commands().begin(),
                                              parser_responses_.input_commands().end(),
                                              "not");
                  auto indexAND = std::find(parser_responses_.input_commands().begin(),
                                            parser_responses_.input_commands().end(),
                                            "and");
                  auto indexOR = std::find(parser_responses_.input_commands().begin(),
                                            parser_responses_.input_commands().end(),
                                            "or");
                  auto indexNULL = std::find(parser_responses_.input_commands().begin(),
                                            parser_responses_.input_commands().end(),
                                            DATATYPES.NULl);
                  if (indexMORE != parser_responses_.input_commands().end()) {
                    auto operand_a = indexMORE - 1;
                    auto operand_b = indexMORE + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] > static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  } else if (indexEQ != parser_responses_.input_commands().end()) {
                    auto operand_a = indexEQ - 1;
                    auto operand_b = indexEQ + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] == static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  } else if (indexLESS != parser_responses_.input_commands().end()) {
                    auto operand_a = indexMORE - 1;
                    auto operand_b = indexMORE + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] < static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  } else if (indexLESSEQ != parser_responses_.input_commands().end()) {
                    auto operand_a = indexMORE - 1;
                    auto operand_b = indexMORE + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] <= static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  } else if (indexMOREEQ != parser_responses_.input_commands().end()) {
                    auto operand_a = indexMORE - 1;
                    auto operand_b = indexMORE + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] >= static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  } else if (indexNOTEQ != parser_responses_.input_commands().end()) {
                    auto operand_a = indexMORE - 1;
                    auto operand_b = indexMORE + 1;
                    std::string column_name = *operand_a;
                    std::string value = *operand_b;
                    if (table_.count(table_name) > 0) {
                      if (table_[table_name].isColumnExists(column_name)) {
                        for (auto j = 0; j < table_[table_name].getColumn(column_name).cells_.size(); ++j) {
                          if (table_[table_name].getColumn(column_name).cells_[j] != static_cast<Table::Value>(value)) {
                            selected_table_.addLine(table_[table_name].getLine(j));
                          }
                        }
                        return;
                      } else {
                        std::cerr << "No such column " << column_name << " in " << table_name << '\n';
                        parser_responses_.change_parsing_status(false);
                        return;
                      }
                    } else {
                      std::cerr << "No such table in DB\n";
                      parser_responses_.change_parsing_status(false);
                      return;
                    }
                  }
                  for (size_t j = i + 4; j < index_FROM - parser_responses_.input_commands().begin(); ++j) {
                    if (table_[*index_existing_table].isColumnExists(parser_responses_.input_commands(j))) {
                      new_table.addColumn(parser_responses_.input_commands(j),
                                          table_[*index_existing_table].getColumn(parser_responses_.input_commands(j)).cells_);
                    } else {
                      std::cerr << "Column '" << parser_responses_.input_commands(j) << "' does not exist in the table."
                                << std::endl;
                      parser_responses_.change_parsing_status(false);
                    }
                  }
                }
              } else {
                std::cerr << "Table '" << *index_existing_table << "' does not exist." << std::endl;
                parser_responses_.change_parsing_status(false);
                return;
              }
              table_[table_name] = new_table;
              all_tables_.push_back(table_name);
            } else {
              std::cerr << syntax_error.description[COMMANDS.FROM] << std::endl;
              parser_responses_.change_parsing_status(false);
            }
          }
        } else {
          std::cerr << syntax_error.description[COMMANDS.SELECT] << std::endl;
          parser_responses_.change_parsing_status(false);
        }
        return;

      } else if (parser_responses_.input_commands(i) == COMMANDS.SELECT) { // SELECT ... FROM table_name
        auto indexFROM = std::find(parser_responses_.input_commands().begin(),
                                   parser_responses_.input_commands().end(),
                                   COMMANDS.FROM);
        if (indexFROM != parser_responses_.input_commands().end()) {
          std::string table_name = *(indexFROM + 1);
          if (table_.count(table_name) > 0) {
            if (*(indexFROM - 1) == "*" && indexFROM - parser_responses_.input_commands().begin() <= 2) {
              parser_responses_.selectTable() = table_[table_name];
              return;
            } else {
              for (size_t j = i + 2; j < indexFROM - parser_responses_.input_commands().begin(); ++j) {
                if (table_[table_name].isColumnExists(parser_responses_.input_commands(j))) {
                  parser_responses_.selectTable().addColumn(parser_responses_.input_commands(j),
                                                            table_[table_name].getColumn(parser_responses_.input_commands(
                                                                j)).cells_);
                } else {
                  std::cerr << "Column '" << parser_responses_.input_commands(j) << "' does not exist in the table."
                            << std::endl;
                  parser_responses_.change_parsing_status(false);
                  return;
                }
              }
              if (std::find(indexFROM, parser_responses_.input_commands().end(), COMMANDS.JOIN)
                  != parser_responses_.input_commands().end()) {
                auto indexON = std::find(indexFROM, parser_responses_.input_commands().end(), COMMANDS.ON);
                if (indexON != parser_responses_.input_commands().end()) {
                  if (parser_responses_.input_commands(indexFROM - parser_responses_.input_commands().begin() + 2)
                      == COMMANDS.LEFT) {
                    joined_table_ = leftJoin(*(indexFROM + 1), *(indexON - 1), *(indexON + 1), *(indexON + 3));
                    if (!parser_responses_.parsing_status()) {
                      return;
                    }
                  } else if (
                      parser_responses_.input_commands(indexFROM - parser_responses_.input_commands().begin() + 2)
                          == COMMANDS.INNER) {
                    joined_table_ = innerJoin(*(indexFROM + 1), *(indexON - 1), *(indexON + 1), *(indexON + 3));
                    if (!parser_responses_.parsing_status()) {
                      return;
                    }
                  } else if (
                      parser_responses_.input_commands(indexFROM - parser_responses_.input_commands().begin() + 2)
                          == COMMANDS.RIGHT) {
                    joined_table_ = rightJoin(*(indexFROM + 1), *(indexON - 1), *(indexON + 1), *(indexON + 3));
                    if (!parser_responses_.parsing_status()) {
                      return;
                    }
                  }
                  return;
                } else {
                  std::cerr << syntax_error.description[COMMANDS.ON] << std::endl;
                  parser_responses_.change_parsing_status(false);
                  return;
                }
              }
            }
          } else {
            std::cerr << "Table '" << table_name << "' does not exist." << std::endl;
            parser_responses_.change_parsing_status(false);
            return;
          }
        } else {
          std::cerr << syntax_error.description[COMMANDS.FROM] << std::endl;
          parser_responses_.change_parsing_status(false);
        }
        return;

      } else if (parser_responses_.input_commands(i) == COMMANDS.DROP) {
        if (parser_responses_.input_commands(i + 1) == COMMANDS.TABLE) {
          if (table_.count(parser_responses_.input_commands(i + 2)) > 0) {
            table_.erase(parser_responses_.input_commands(i + 2));
            all_tables_.erase(std::find(all_tables_.begin(),
                                        all_tables_.end(),
                                        parser_responses_.input_commands(i + 3)));
            return;
          } else {
            std::cerr << "Table '" << parser_responses_.input_commands(i + 2) << "' doesn't exist." << std::endl;
            parser_responses_.change_parsing_status(false);
            return;
          }
        } else {
          parser_responses_.change_parsing_status(false);
          return;
        }
      } else if (parser_responses_.input_commands(i) == COMMANDS.DELETE) {
        if (parser_responses_.input_commands(i + 1) == COMMANDS.FROM) {
          if (table_.count(parser_responses_.input_commands(i + 2)) > 0) {
            table_[parser_responses_.input_commands(i + 2)].deleteColumns();
          } else {
            std::cerr << "Table '" << parser_responses_.input_commands(i + 2) << "' doesn't exist." << std::endl;
            parser_responses_.change_parsing_status(false);
            return;
          }
        }
      } else if (parser_responses_.input_commands(i) == COMMANDS.INSERT
          && parser_responses_.input_commands(i + 1) == COMMANDS.INTO) {
        std::string table_name = parser_responses_.input_commands(i + 2);
        auto indexVALUES = std::find(parser_responses_.input_commands().begin(),
                                     parser_responses_.input_commands().end(),
                                     COMMANDS.VALUES);
        if (indexVALUES != parser_responses_.input_commands().end()) {
          for (size_t j = i + 3, k = 1; j < indexVALUES - parser_responses_.input_commands().begin(); ++j, ++k) {
            if (table_[table_name].isColumnExists(parser_responses_.input_commands(j))) {
              table_[table_name].getColumn(parser_responses_.input_commands(j)).cells_.push_back(static_cast<Table::Value>(parser_responses_.input_commands(
                  indexVALUES - parser_responses_.input_commands().begin() + k)));
            }
          }
        } else {
          std::cerr << syntax_error.description[COMMANDS.INSERT] << std::endl;
          parser_responses_.change_parsing_status(false);
          return;
        }
      } else if (parser_responses_.input_commands(i) == COMMANDS.UPDATE) {
        std::cerr << "No need using UPDATE without SET\n";
        parser_responses_.change_parsing_status(false);
        return;
      }
    }
    auto ind = std::find(parser_responses_.input_commands().begin(),
                         parser_responses_.input_commands().end(),
                         COMMANDS.PRIMARY);
    if (ind != parser_responses_.input_commands().end()) {
      if (*(ind + 1) == COMMANDS.KEY) {
        auto indexTABLE = std::find(parser_responses_.input_commands().begin(),
                                    parser_responses_.input_commands().end(),
                                    COMMANDS.TABLE);
        if (indexTABLE != parser_responses_.input_commands().end()) {
          auto table_name = *(indexTABLE + 1);
          if (table_.count(table_name)) {
            auto CurrentTable = table_[table_name];
            if (CurrentTable.isColumnExists(*(ind - 1))) {
              CurrentTable.getColumn(*(ind - 1)).make_primary();
            } else {
              std::cerr << "The column " << *(ind - 1) << " doesn't exists in the table " << CurrentTable.getName()
                        << '\n';
              parser_responses_.change_parsing_status(false);
              return;
            }
          } else {
            std::cerr << "Table doesn't exists in DB\n";
            parser_responses_.change_parsing_status(false);
            return;
          }
        } else {
          std::cerr << "expected Table name\n";
          parser_responses_.change_parsing_status(false);
          return;
        }
      }
    }
  }

  const Table& joined_table() const {
    return joined_table_;
  }

 private:
  std::string name_;
  ParserResponsesSQL parser_responses_;
  std::vector<std::string> all_tables_;
  std::map<std::string, Table> table_;
  Table selected_table_;
  Table joined_table_;
 public:

  friend std::ostream& operator<<(std::ostream& ostream, const MyCoolDB& db) {
    ostream << "Database name: " << db.name_ << std::endl;
    ostream << "Parser response: " << db.parser_responses_.request() << std::endl;
    ostream << "Parsing status: " << db.parsing_status() << std::endl;
    ostream << "Tables: " << std::endl;
    for (const auto& name : db.all_tables_) {
      ostream << name << ", ";
    }
    ostream << std::endl;
    ostream << std::endl;
    for (const auto& table : db.table_) {
      ostream << "Table: " << table.first << ": " << std::endl;
      for (const auto& name : table.second.getColumnNames()) {
        ostream << name << " | ";
      }
      for (size_t i = 0; i < table.second.getColumns()[0].cells_.size(); ++i) {
        auto line = table.second.getLine(i);
        for (const auto& i : line) {
          ostream << i << " | ";
        }
      }
    }

    return ostream;
  }

  Table innerJoin(const std::string& table1,
                  const std::string& table2,
                  const std::string& column1,
                  const std::string& column2) {
    Table result;
    if (table_.count(table1) == 0 || table_.count(table2) == 0) {
      std::cerr << "One or both tables do not exist." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table& t1 = table_[table1];
    const Table& t2 = table_[table2];

    if (!t1.isColumnExists(column1) || !t2.isColumnExists(column2)) {
      std::cerr << "One or both columns do not exist in the tables." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table::Column& c1 = t1.getColumn(column1);
    const Table::Column& c2 = t2.getColumn(column2);

    for (size_t i = 0; i < c1.cells_.size(); ++i) {
      const Table::Value& v1 = c1.cells_[i];
      for (size_t j = 0; j < c2.cells_.size(); ++j) {
        const Table::Value& v2 = c2.cells_[j];
        if (v1 == v2) {
          std::vector<Table::Value> line;
          for (const std::string& columnName : t1.getColumnNames()) {
            line.push_back(t1.getColumn(columnName).cells_[i]);
          }
          for (const std::string& columnName : t2.getColumnNames()) {
            line.push_back(t2.getColumn(columnName).cells_[j]);
          }
          result.addLine(line);
        }
      }
    }
    return result;
  }

  Table leftJoin(const std::string& table1,
                 const std::string& table2,
                 const std::string& column1,
                 const std::string& column2) {
    Table result;
    if (table_.count(table1) == 0 || table_.count(table2) == 0) {
      std::cerr << "One or both tables do not exist." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table& t1 = table_[table1];
    const Table& t2 = table_[table2];

    if (!t1.isColumnExists(column1) || !t2.isColumnExists(column2)) {
      std::cerr << "One or both columns do not exist in the tables." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table::Column& c1 = t1.getColumn(column1);
    const Table::Column& c2 = t2.getColumn(column2);

    for (size_t i = 0; i < c1.cells_.size(); ++i) {
      const Table::Value& v1 = c1.cells_[i];
      bool match_found = false;
      for (size_t j = 0; j < c2.cells_.size(); ++j) {
        const Table::Value& v2 = c2.cells_[j];
        if (v1 == v2) {
          match_found = true;
          std::vector<Table::Value> line;
          for (const std::string& columnName : t1.getColumnNames()) {
            line.push_back(t1.getColumn(columnName).cells_[i]);
          }
          for (const std::string& columnName : t2.getColumnNames()) {
            line.push_back(t2.getColumn(columnName).cells_[j]);
          }
          result.addLine(line);
        }
      }
      if (!match_found) {
        std::vector<Table::Value> line;
        for (const std::string& columnName : t1.getColumnNames()) {
          line.push_back(t1.getColumn(columnName).cells_[i]);
        }
        for (const std::string& columnName : t2.getColumnNames()) {
          line.push_back(Table::Value());
        }
        result.addLine(line);
      }
    }

    return result;
  }

  Table rightJoin(const std::string& table1,
                  const std::string& table2,
                  const std::string& column1,
                  const std::string& column2) {
    Table result;
    if (table_.count(table1) == 0 || table_.count(table2) == 0) {
      std::cerr << "One or both tables do not exist." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table& t1 = table_[table1];
    const Table& t2 = table_[table2];

    if (!t1.isColumnExists(column1) || !t2.isColumnExists(column2)) {
      std::cerr << "One or both columns do not exist in the tables." << std::endl;
      parser_responses_.change_parsing_status(false);
      return result;
    }

    const Table::Column& c1 = t1.getColumn(column1);
    const Table::Column& c2 = t2.getColumn(column2);

    for (size_t j = 0; j < c2.cells_.size(); ++j) {
      const Table::Value& v2 = c2.cells_[j];
      bool match_found = false;
      for (size_t i = 0; i < c1.cells_.size(); ++i) {
        const Table::Value& v1 = c1.cells_[i];
        if (v1 == v2) {
          match_found = true;
          std::vector<Table::Value> line;
          for (const std::string& columnName : t1.getColumnNames()) {
            line.push_back(t1.getColumn(columnName).cells_[i]);
          }
          for (const std::string& columnName : t2.getColumnNames()) {
            line.push_back(t2.getColumn(columnName).cells_[j]);
          }
          result.addLine(line);
        }
      }
      if (!match_found) {
        std::vector<Table::Value> line;
        for (const std::string& columnName : t1.getColumnNames()) {
          line.push_back(Table::Value());
        }
        for (const std::string& columnName : t2.getColumnNames()) {
          line.push_back(t2.getColumn(columnName).cells_[j]);
        }
        result.addLine(line);
      }
    }

    return result;
  }

};

};