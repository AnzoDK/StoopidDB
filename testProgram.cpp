#include <iostream>
#include "stoopidDB.h"

void RowPrinter(DBRow* row, int arraySize)
{
    for(size_t i = 0; i < arraySize;i++)
    {
         for(size_t u = 0; u < row[i].keys.size();u++)
         std::cout << "DB Returned key: " << row[i].keys.at(u).name << " : " << row[i].keys.at(u).value << std::endl;
    }
}

int main()
{
    DBManager* dbMan = new DBManager();
    dbMan->CreateDB("./local_db.sdb");
    dbMan->LoadDB("./local_db.sdb");
    DBColumn* columns = new DBColumn[3];
    columns[0] = DBColumn("id",DataType::INT, 8, ColumnSettings::NOT_NULL | ColumnSettings::AUTO_INCREMENT | ColumnSettings::PRIMARY_KEY);
    columns[1] = DBColumn("Name", DataType::VARCHAR, 50, ColumnSettings::NOT_NULL);
    columns[2] = DBColumn("Age", DataType::INT, 8, ColumnSettings::NOT_NULL);
    if(dbMan->CreateTable("Users",3,columns))
    {
        delete[] columns;
        DBRow row = DBRow();
        row.InsertData("Name","John Doe");
        row.InsertData("Age","35");
        if(!dbMan->InsertRow("Users",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
    }
    columns = new DBColumn[3];
    columns[0] = DBColumn("id",DataType::INT, 8, ColumnSettings::NOT_NULL | ColumnSettings::AUTO_INCREMENT | ColumnSettings::PRIMARY_KEY);
    columns[1] = DBColumn("Title", DataType::VARCHAR, 100, ColumnSettings::NOT_NULL);
    columns[2] = DBColumn("Category", DataType::VARCHAR, 8, ColumnSettings::NOT_NULL);
    if(dbMan->CreateTable("Posts",3,columns))
    {
        delete[] columns;
        DBRow row = DBRow();
        row.InsertData("Title","A title is all you need");
        row.InsertData("Category","TextPosts");
        if(!dbMan->InsertRow("Posts",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
        row = DBRow();
        row.InsertData("Title","The more titles the better");
        row.InsertData("Category","TextPosts");
        if(!dbMan->InsertRow("Posts",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
        row = DBRow();
        row.InsertData("Title","The last title");
        row.InsertData("Category","TextPosts");
        if(!dbMan->InsertRow("Posts",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
    }
    int resultSize = 0;
    std::cout << "Printing table: Users" << std::endl; 
    DBRow* result = dbMan->GetAllRowsFromTable("Users",resultSize);
    RowPrinter(result,resultSize);
    delete[] result;
    result = dbMan->GetAllRowsFromTable("Posts",resultSize);
    std::cout << "Printing table: Titles" << std::endl; 
    RowPrinter(result,resultSize);
    delete[] result;
    Key conditions[1] = {Key("id","0")};
    uint64_t bit64ResultSize = 0;
    result = dbMan->GetRowsWhere("Posts",conditions,1,bit64ResultSize);
    std::cout << "Printing result of 'GetRowsWhere Condition: ID=0'" << std::endl;
    RowPrinter(result,bit64ResultSize);
    delete[] result;
    dbMan->DeleteTable("Users");
#ifndef NO_SQL
    if(dbMan->SQlQuery("CREATE TABLE Hats(ID int(8) NOT NULL, Size int(8) NOT NULL, PRIMARY KEY (ID)); SELECT ( Title, id ) FROM Posts;",1).code != SQLResponseCode::SQL_OK)
    {
        std::cout << TERMINAL_RED << "Error - as expected" << std::endl;
        std::cout << dbMan->Error() << TERMINAL_NOCOLOR << std::endl;
    }
    else
    {
        std::cout << "Printing output of 'SELECT ( Title, id ) FROM Posts;'" << std::endl;
        RowPrinter(dbMan->SqlReturn,dbMan->SqlReturnLength);
        DBRow row = DBRow();
        row.InsertData("Size","12");
        std::cout << TERMINAL_GREEN << "No error returned by SQL handler - Testing Database against expected outputs..." << TERMINAL_NOCOLOR<< std::endl;
        std::cout << TERMINAL_CYAN << "Inserting test row containg: Column: (Size) = " << row.Find("Size").value << std::endl;
        if(dbMan->InsertRow("Hats",row))
        {
            std::cout << "Row inserted - Reading row back..." << std::endl;
            int rowsSize = 0;
            DBRow* rows = dbMan->GetAllRowsFromTable("Hats",rowsSize);
            if(rowsSize > 0)
            {  
                for(int u = 0; u < rows[0].keys.size(); u++)
                {
                    if(rows[0].keys.at(u).name == "Size")
                    {
                        std::cout << "ColumnName: " << rows[0].keys.at(u).name << "; ColumnValue: " << rows[0].keys.at(u).value << " - Expected(" << row.Find("Size").value << ")" << std::endl;
                        if(rows[0].keys.at(u).value == row.Find("Size").value)
                        {
                            std::cout << TERMINAL_GREEN << "Coulumn Match!" << TERMINAL_CYAN << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "ColumnName: " << rows[0].keys.at(u).name << "; ColumnValue: " << rows[0].keys.at(u).value << std::endl;
                    }
                }
            }
            else
            {
                
                std::cout << TERMINAL_RED << "[FATAL] Database did not return the inserted row.." << TERMINAL_NOCOLOR << std::endl;
                delete dbMan;
                return 1;
            }
            
        }
        else
        {
            std::cout << TERMINAL_RED << "[FATAL] Row insertion failed with error: " << dbMan->Error() << TERMINAL_NOCOLOR << std::endl;
            delete dbMan;
            return 1;
        }
    }
#endif
    
    delete dbMan;
}
