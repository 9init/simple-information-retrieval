#include <iostream>
#include <fstream>
#include <map>
#include <dirent.h>
#include <string>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;


struct IR_TERM{
     std::string term;
     int docs_no;
     std::map<string, set<int>> docs;
};

struct IR{
     std::set<std::string> stopword;
     std::map<string, IR_TERM> terms;
};

std::string READ_STOP_FILE = "READ_STOP_FILE";
std::string READ_NORMAL_FILE = "READ_NORMAL_FILE";

void addTerm(std::string term, std::string filename, int position, IR *ir){
     if(ir->terms.find(term) == ir->terms.end()){
          IR_TERM newIR_Term;
          newIR_Term.term = term;
          newIR_Term.docs_no++;
          std::set<int> v;
          v.insert(position);
          newIR_Term.docs.insert({filename, v});
          ir->terms.insert({term, newIR_Term});
     }else{
          ir->terms.at(term).docs_no++;
          if(ir->terms.at(term).docs.find(filename) == ir->terms.at(term).docs.end()){
               std::set<int> v;
               v.insert(position);
               ir->terms.at(term).docs.insert({filename, v});
          }else
               ir->terms.at(term).docs.at(filename).insert(position);
     }
}


bool checkValidity(std::string str, IR *ir){
     return str.length() > 2 && (ir->stopword.find(str) == ir->stopword.end());
}

void methodHandler(std::string word, std::string filename, int position, std::string method, IR *ir){
          if(method == READ_NORMAL_FILE && checkValidity(word, ir))
               addTerm(word, filename, position, ir);
          else if(method == READ_STOP_FILE)
               ir->stopword.insert(word);
}

void toLowerCase(std::string *str){
     std::transform(str->begin(), str->end(), str->begin(), [](unsigned char c){ return std::tolower(c); });
}

void handelWords( std::string line, std::string filename, int position, std::string method, IR *ir) {
     std::istringstream iss(line);
     std::string word;
     while(iss >> word){
          toLowerCase(&word);
          methodHandler(word, filename, position, method, ir);
     }
}


void read_file(std::string filename, std::string method, IR *ir){
     std::ifstream infile(filename);
     int position;
     for( std::string line; getline( infile, line ); ){
          position++;
          handelWords(line, filename, position, method, ir);
     }
}


void read_directory(const std::string path, IR *ir){
     DIR *dirp = opendir(path.c_str());
     struct dirent *dp;
     while ((dp = readdir(dirp)) != NULL) {
          read_file(path + "/" + dp->d_name, READ_NORMAL_FILE, ir);
     }
     closedir(dirp);
}

void search(std::string term, IR *ir){
     toLowerCase(&term);
     if(ir->terms.find(term) == ir->terms.end()){
          cout << "nothing here" << endl;
          return;
     }


     int docsNum = ir->terms.at(term).docs_no;
     for (auto &x : ir->terms.at(term).docs){
          printf("%s [ ", x.first.c_str());

          for(auto it : x.second){
               printf("%d ", it);
          }
          printf("]\n");
     }
}




int main(){
     cout << "start sorting" << endl;
     IR ir;
     //reading stop word
     read_file("/home/ahmed/Desktop/IR/stop-word.txt", READ_STOP_FILE, &ir);

     //Docs
     read_directory("/home/ahmed/Desktop/IR/docs", &ir);

     while(true){
          std::string str;
          cout << "search for what? : ";
          cin >> str;
          search(str, &ir);
     }
     return 0;
}
