#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

const int INIT_VISIT_NUM = 0;
const int HOUR = 60;
const int CLOCK_THRESHOLD = 10;
const int END_OF_LINE = -1;
const char DATA_SEPERATOR = ',';
const char DAY_SEPERATOR = '$';
const char DASH = '-';
const char END_OF_CSV_LINE = '\r';
const string TIME_NOTATION = ":";
const string ZERO_CLOCK = "0";
const string SATURDAY = "Sat";
const string SUNDAY = "Sun";
const string MONDAY = "Mon";
const string TUESDAY = "Tue";
const string WEDNESDAY = "Wed";
const string THURSDAY = "Thu";
const string FRIDAY = "Fri";
const string NAME = "Name: ";
const string DOCTOR = "Doctor: ";
const string VISIT = "Visit: ";
const string CHARGE = "Charge: ";
const string NO_FREE_TIME = "No free time";
const string SEPERATE_RESULTS = "----------";
const string SPACE = " ";


struct DailyVisit
{
    int Sat = INIT_VISIT_NUM;
    int Sun = INIT_VISIT_NUM;
    int Mon = INIT_VISIT_NUM;
    int Tue = INIT_VISIT_NUM;
    int Wed = INIT_VISIT_NUM;
    int Thu = INIT_VISIT_NUM;
    int Fri = INIT_VISIT_NUM;
};


struct Patient
{
    string name;
    string mediacl_issue;
    string appointment_time;
    string reserved_doctor;
    string reserved_day;
    string reserved_time;
    int reserved_fee;
    int visit_number;
    string specialty;
};


struct Doctor
{
    string name;
    string specialty;
    int visit_fee;
    int visit_duration;
    int wait_time_average;
    vector<string> days_available;
    DailyVisit visit_count;
};


struct Disease
{
    string specialty;
    vector<string> disease;
};


vector<string> split_text(string text, char seperator){
    vector<string> result;
    string token;
    int start = 0;
    int end = text.find(seperator);
    while (end != END_OF_LINE) {
        token = text.substr(start, end - start);
        result.push_back(token);
        start = end + 1;
        end = text.find(seperator, start);
    }
    token = text.substr(start, end);
    result.push_back(token);
    return result;
}


vector<Patient> read_patients_csv(string file_name){
    ifstream patients_file(file_name);
    vector<Patient> patients;
    string line;
    getline(patients_file, line);
    while (getline(patients_file, line))
    {
        if (!line.empty() && line.back() == END_OF_CSV_LINE) {
            line.pop_back();
        }
        vector<string> data = split_text(line, DATA_SEPERATOR);
        patients.push_back({data[0], data[1], data[2]});
    }
    patients_file.close();
    return patients;
}


vector<Doctor> read_docters_csv(string file_name){
    ifstream doctors_file(file_name);
    vector<Doctor> doctors;
    string line;
    getline(doctors_file, line);
    while (getline(doctors_file, line))
    {
        vector<string> data = split_text(line, DATA_SEPERATOR);
        vector<string> days_available = split_text(data[5], DAY_SEPERATOR);
        if (days_available[days_available.size() - 1].back() == END_OF_CSV_LINE) {
            days_available[days_available.size() - 1].pop_back();
        }
        doctors.push_back({data[0], data[1], stoi(data[2]), stoi(data[3]), stoi(data[4]), days_available});
    }
    doctors_file.close();
    return doctors;
}


vector<Disease> read_disease_csv(string file_name){
    ifstream diseases_file(file_name);
    vector<Disease> diseases;
    string line;
    getline(diseases_file, line);
    while (getline(diseases_file, line))
    {
        vector<string> data = split_text(line, DATA_SEPERATOR);
        vector<string> diseases_name = split_text(data[1], DAY_SEPERATOR);
        if (diseases_name[diseases_name.size() - 1].back() == END_OF_CSV_LINE) {
            diseases_name[diseases_name.size() - 1].pop_back();
        }
        diseases.push_back({data[0], diseases_name});
    }
    diseases_file.close();
    return diseases;
}


bool compare_patient(const Patient& p1, const Patient& p2) {
    map<string, int> day_order = {{SATURDAY, 0},{SUNDAY, 1}, {MONDAY, 2}, {TUESDAY, 3}, {WEDNESDAY, 4}, {THURSDAY, 5}, {FRIDAY, 6}};
    string day1 = p1.appointment_time.substr(0,3);
    int time1 = stoi(p1.appointment_time.substr(4));
    string day2 = p2.appointment_time.substr(0,3);
    int time2 = stoi(p2.appointment_time.substr(4));
    if(p1.appointment_time == p2.appointment_time)
        return  p1.name < p2.name;
    else if (day_order[day1] == day_order[day2])
        return time1 < time2;
    else return day_order[day1] < day_order[day2];
}


bool compare_doctor_days(const string& str1, const string& str2) {
    vector<string> str1_vec = split_text(str1, DASH);
    vector<string> str2_vec = split_text(str2, DASH);
    map<string, int> day_order = {{SATURDAY, 0},{SUNDAY, 1}, {MONDAY, 2}, {TUESDAY, 3}, {WEDNESDAY, 4}, {THURSDAY, 5}, {FRIDAY, 6}};
    return day_order[str1_vec[0]] < day_order[str2_vec[0]];
}


void sort_petient_for_reservation(vector<Patient> & patient_list){
    sort(patient_list.begin(), patient_list.end(), compare_patient);
}


bool is_day_empty(Doctor& doctor, string day, int arrive, int depart, int visit_dur){
    int num_of_patient = ((depart - arrive) * HOUR) / visit_dur;
    bool is_found = 1;
    if(day == SATURDAY && doctor.visit_count.Sat < num_of_patient)
        doctor.visit_count.Sat ++;
    else if(day == SUNDAY && doctor.visit_count.Sun < num_of_patient)
        doctor.visit_count.Sun ++;
    else if(day == MONDAY && doctor.visit_count.Mon < num_of_patient)
        doctor.visit_count.Mon ++;
    else if(day == TUESDAY && doctor.visit_count.Tue < num_of_patient)
        doctor.visit_count.Tue ++;
    else if(day == WEDNESDAY && doctor.visit_count.Wed < num_of_patient)
        doctor.visit_count.Wed ++;
    else if(day == THURSDAY && doctor.visit_count.Thu < num_of_patient)
        doctor.visit_count.Thu ++;
    else if(day == FRIDAY && doctor.visit_count.Fri < num_of_patient)
        doctor.visit_count.Fri ++;
    else is_found = 0;
    return is_found;
}


bool is_day_full(Doctor& doctor, string day, int arrive, int depart, int visit_dur){
    int num_of_patient = ((depart - arrive) * HOUR) / visit_dur;
    bool is_full = 0;
    if(day == SATURDAY && doctor.visit_count.Sat == num_of_patient)
        is_full = 1;
    else if(day == SUNDAY && doctor.visit_count.Sun == num_of_patient)
        is_full = 1;
    else if(day == MONDAY && doctor.visit_count.Mon == num_of_patient)
        is_full = 1;
    else if(day == TUESDAY && doctor.visit_count.Tue == num_of_patient)
        is_full = 1;
    else if(day == WEDNESDAY && doctor.visit_count.Wed == num_of_patient)
        is_full = 1;
    else if(day == THURSDAY && doctor.visit_count.Thu == num_of_patient)
        is_full = 1;
    else if(day == FRIDAY && doctor.visit_count.Fri == num_of_patient)
        is_full = 1;
    else is_full = 0;
    return is_full;
}


string calc_reserved_time(Doctor doctor, int arrival, int visit_number){
    int min = (visit_number - 1) * doctor.visit_duration;
    int hour = arrival;
    if(min >= HOUR){
        hour += min / HOUR;
        min %= HOUR;
    }
    string min_str = to_string(min);
    string hour_str = to_string(hour);
    if(min < CLOCK_THRESHOLD)
        min_str = ZERO_CLOCK + to_string(min);
    if(hour < CLOCK_THRESHOLD)
        hour_str = ZERO_CLOCK + to_string(hour);
    return hour_str + TIME_NOTATION + min_str;
}


void set_patient_reservation_info(Patient& patient, string reserved_day, Doctor doctor, int arrival){
    int visit_number;
    patient.reserved_doctor = doctor.name;
    patient.reserved_day = reserved_day;
    patient.reserved_fee = doctor.visit_fee;
    if(reserved_day == SATURDAY)
        visit_number = doctor.visit_count.Sat;
    else if(reserved_day == SUNDAY)
        visit_number = doctor.visit_count.Sun;
    else if(reserved_day == MONDAY)
        visit_number = doctor.visit_count.Mon;
    else if(reserved_day == TUESDAY)
        visit_number = doctor.visit_count.Tue;
    else if(reserved_day == WEDNESDAY)
        visit_number = doctor.visit_count.Wed;
    else if(reserved_day == THURSDAY)
        visit_number = doctor.visit_count.Thu;
    else if(reserved_day == FRIDAY)
        visit_number = doctor.visit_count.Fri;
    patient.visit_number = visit_number;
    patient.reserved_time = calc_reserved_time(doctor, arrival, visit_number);
}


bool check_doctor_time(Patient & patient, Doctor& doctor){
    for (int i = 0; i < doctor.days_available.size(); i++)
    {
        vector<string> splited_text = split_text(doctor.days_available[i], DASH);
        if(is_day_empty(doctor, splited_text[0], stoi(splited_text[1]), stoi(splited_text[2]), doctor.visit_duration)){
            set_patient_reservation_info(patient, splited_text[0], doctor, stoi(splited_text[1]));
            if(is_day_full(doctor, splited_text[0], stoi(splited_text[1]), stoi(splited_text[2]), doctor.visit_duration)){
                doctor.days_available.erase(doctor.days_available.begin());
            }
            return true;
        }
    }
    return false;
}


void update_drs_visit_cnt(Doctor sample_doctor, vector<Doctor>& doctors_list){
    for (int i = 0; i < doctors_list.size(); i++)
    {
        if(sample_doctor.name == doctors_list[i].name){
            doctors_list[i].visit_count = sample_doctor.visit_count;   
            doctors_list[i].days_available = sample_doctor.days_available;
        }
    }
    
}


int reserved_num(Doctor doctor, string day){
    if(day == SATURDAY)
        return doctor.visit_count.Sat;
    else if(day == SUNDAY)
        return doctor.visit_count.Sun;
    else if(day == MONDAY)
        return doctor.visit_count.Mon;
    else if(day == TUESDAY)
         return doctor.visit_count.Tue;
    else if(day == WEDNESDAY)
        return doctor.visit_count.Wed;
    else if(day == THURSDAY)
        return doctor.visit_count.Thu;
    else if(day == FRIDAY)
        return doctor.visit_count.Fri;
    return 0;
}


bool compare_doctors(const Doctor& d1, const Doctor& d2) {
    if(d1.days_available.size() != 0 && d2.days_available.size() != 0){
        vector<string> str1_vec = split_text(d1.days_available[0], DASH);
        vector<string> str2_vec = split_text(d2.days_available[0], DASH);
        map<string, int> day_order = {{SATURDAY, 0},{SUNDAY, 1}, {MONDAY, 2}, {TUESDAY, 3}, {WEDNESDAY, 4}, {THURSDAY, 5}, {FRIDAY, 6}};
        if(d1.days_available[0].substr(0,3) != d2.days_available[0].substr(0,3)){
            return day_order[str1_vec[0]] < day_order[str2_vec[0]];
        }
        else if(d1.days_available[0].substr(0,3) == d2.days_available[0].substr(0,3)){
            int zamane_nobate_badi_1 = stoi(str1_vec[1])*HOUR + reserved_num(d1, str1_vec[0])*d1.visit_duration;
            int zamane_nobate_badi_2 = stoi(str2_vec[1])*HOUR + reserved_num(d2, str2_vec[0])*d2.visit_duration;
            if(zamane_nobate_badi_1 != zamane_nobate_badi_2)
                return zamane_nobate_badi_1 < zamane_nobate_badi_2;
        }
    }
    if(d1.visit_fee != d2.visit_fee)
        return d1.visit_fee < d2.visit_fee;
    else if (d1.wait_time_average != d2.wait_time_average) 
        return d1.wait_time_average < d2.wait_time_average;
    else 
        return d1.name < d2.name;
    return false;
}


void sort_doctors(vector<Doctor> & doctors_list){
    for (int i = 0; i < doctors_list.size(); i++)
       sort(doctors_list[i].days_available.begin(), doctors_list[i].days_available.end(), compare_doctor_days); 
    sort(doctors_list.begin(), doctors_list.end(), compare_doctors);
}


void nobat_dehi(vector<Doctor> &doctors_with_the_specialty, Patient& patient, vector<Doctor>& doctors_list){
    for (int i = 0; i < doctors_with_the_specialty.size(); i++)
    {
        if(check_doctor_time(patient, doctors_with_the_specialty[i])){
            update_drs_visit_cnt(doctors_with_the_specialty[i], doctors_list);
            return;
        }
    }
    patient.visit_number = -1;
}


void find_doctor_for_each_patient(Patient& patient, vector<Doctor>& doctors_list){
    vector<Doctor> doctors_with_the_specialty;
    for (int i = 0; i < doctors_list.size(); i++){
        if(patient.specialty == doctors_list[i].specialty){
            doctors_with_the_specialty.push_back(doctors_list[i]);
        }
    }
    sort_doctors(doctors_with_the_specialty);
    nobat_dehi(doctors_with_the_specialty, patient, doctors_list);
}


void find_doctor_for_all_patient(vector<Patient>& patients_list, vector<Doctor>& doctors_list){
    for(int i = 0; i<patients_list.size(); i++)
        find_doctor_for_each_patient(patients_list[i], doctors_list);
}


void find_each_patient_specialty(Patient& patient, vector<Disease> diseases_list){
    for(int i = 0; i<diseases_list.size(); i++){
        for(int j = 0; j<diseases_list[i].disease.size(); j++){
            if(patient.mediacl_issue == diseases_list[i].disease[j])
                patient.specialty = diseases_list[i].specialty;
        }
    }
}


void find_all_petient_specialty(vector<Patient>& patients_list, vector<Disease> diseases_list){
    for(int i = 0; i<patients_list.size(); i++)
        find_each_patient_specialty(patients_list[i], diseases_list);
}


void schedulate(vector<Patient>& patients_list, vector<Doctor>& doctors_list, vector<Disease> disease_list){
    find_all_petient_specialty(patients_list, disease_list);
    find_doctor_for_all_patient(patients_list, doctors_list);
}


bool compare_patient_by_name(const Patient& P1, const Patient& P2) {
    return P1.name < P2.name;
}


void print_reservations(vector<Patient> patients_list){
    sort(patients_list.begin(), patients_list.end(), compare_patient_by_name);
    for (int i = 0; i < patients_list.size(); i++)
    {
        cout << NAME << patients_list[i].name << endl;
        if(patients_list[i].visit_number == -1){
            cout << NO_FREE_TIME << endl;
        }
        else{
            cout << DOCTOR << patients_list[i].reserved_doctor << endl;
            cout << VISIT << patients_list[i].reserved_day << SPACE << patients_list[i].visit_number << SPACE << patients_list[i].reserved_time << endl;
            cout << CHARGE << patients_list[i].reserved_fee << endl;
        }
        if(i != patients_list.size() - 1)
            cout << SEPERATE_RESULTS << endl;
    }
}


int main(){
    vector<Patient> patients_list = read_patients_csv("patients.csv");
    vector<Doctor> doctors_list = read_docters_csv("doctors.csv");
    vector<Disease> disease_list = read_disease_csv("diseases.csv");
    sort_petient_for_reservation(patients_list);
    schedulate(patients_list, doctors_list, disease_list);
    print_reservations(patients_list);
    return 0;
}