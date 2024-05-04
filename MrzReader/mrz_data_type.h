#pragma once
#include <string>


	constexpr int country_len = 4;
	constexpr int last_name_len = 21;
	constexpr int given_name_len = 31;
	constexpr int document_number_len = 12;
	constexpr int nationality_len = 4;
	constexpr int brith_date_len = 12;
	constexpr int sex_len = 2;
	constexpr int expiry_date_len = 12;

	constexpr int textLens[] = { country_len, last_name_len, given_name_len,document_number_len,nationality_len,
		brith_date_len, sex_len, expiry_date_len };

	struct MrzData {
		char country_[country_len];
		char last_name_[last_name_len];
		char given_name_[given_name_len];


		char document_number_[document_number_len];
		char nationality_[nationality_len];

		char brith_date_[brith_date_len];
		char sex_[sex_len];

		char expiry_date_[expiry_date_len];


	};
	void clearMrzData(MrzData& value);
	void printMrzData(const MrzData& mrz_data);