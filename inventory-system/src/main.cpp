#include <iostream>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>

int main() {
    try {
        // 建立連接
        soci::session sql(soci::mysql, 
            "db=inventory user=your_user password=your_password");
        
        std::cout << "Successfully connected to database!" << std::endl;
        
    } catch (const soci::mysql_soci_error& e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}