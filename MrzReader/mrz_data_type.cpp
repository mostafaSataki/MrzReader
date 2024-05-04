#include "mrz_data_type.h"
#include <iostream>
using namespace std;

void clearMrzData(MrzData& data)
{

		memset(data.country_, 0, sizeof(data.country_));
		memset(data.last_name_, 0, sizeof(data.last_name_));
		memset(data.given_name_, 0, sizeof(data.given_name_));
		memset(data.document_number_, 0, sizeof(data.document_number_));
		memset(data.nationality_, 0, sizeof(data.nationality_));
		memset(data.brith_date_, 0, sizeof(data.brith_date_));
		memset(data.sex_, 0, sizeof(data.sex_));
		memset(data.expiry_date_, 0, sizeof(data.expiry_date_));


}
void printMrzData(const MrzData& mrz_data)
{
	cout << "country : " << mrz_data.country_ << endl;
	cout << "last_name : " << mrz_data.last_name_ << endl;
	cout << "given_name : " << mrz_data.given_name_ << endl;
	cout << "document_number : " << mrz_data.document_number_ << endl;
	cout << "nationality : " << mrz_data.nationality_ << endl;
	cout << "brith_date : " << mrz_data.brith_date_ << endl;

	cout << "sex : " << mrz_data.sex_ << endl;
	cout << "expiry_date_ : " << mrz_data.expiry_date_ << endl;



}