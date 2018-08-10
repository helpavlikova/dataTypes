#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <exception>
#include <typeinfo>

#if defined ( __cplusplus ) && __cplusplus > 199711L	/* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */
using namespace std;

class CSyntaxException:public exception
{
public:
  //---------------------------------------------------------------------------------------------
  CSyntaxException (const string & desc):m_Desc (desc)
  {
  }
  //---------------------------------------------------------------------------------------------
  virtual const char *what (void) const noexcept
  {
    return m_Desc.c_str ();
  }
  //---------------------------------------------------------------------------------------------
private:
    string m_Desc;
};
#endif /* __PROGTEST__ */

class CType{
 public:
    CType() {}
    virtual ~CType();
    virtual size_t Sizeof() const = 0;
    virtual CType* clone() const = 0; 
    virtual void Print(ostream& os) const = 0;
    bool operator==(const CType& a);
    bool operator!=(const CType& a);
    virtual bool Equals(const CType& a);
    friend ostream& operator<<(ostream& os, const CType& sth);
 private:
    size_t size;
    string name;
};


ostream& operator<<(ostream& os, const CType& sth){
  sth.Print(os);
  return os;
}

bool CType::Equals(const CType& a){
    return (typeid(a) == typeid(*this));
}

bool CType::operator==(const CType& a){
    return this->Equals(a);
}

bool CType::operator!=(const CType& a){
    return !(operator==(a));
}

CType::~CType(){
    //cout << "destrukce CType" << endl;
}

//--------------------------------------------------------------------------------
class CDataTypeInt : public CType {
 public:
     CDataTypeInt(){}
    virtual ~CDataTypeInt();
    virtual size_t Sizeof() const {return size;}
    virtual CDataTypeInt* clone() const; 
    virtual void Print(ostream& os) const;
    virtual bool Equals(const CType& a);
private:
    size_t size = 4;
};

bool CDataTypeInt::Equals(const CType& a){
    return CType::Equals(a);
}

 void CDataTypeInt::Print(ostream& os) const{
     os << "int "; 
 }

CDataTypeInt* CDataTypeInt::clone() const{
    return new CDataTypeInt(*this);
}
CDataTypeInt::~CDataTypeInt(){
   // cout << "destrukce int" << endl;
}

//--------------------------------------------------------------------------------
class CDataTypeDouble: public CType {
public:
  
     CDataTypeDouble(){}
    virtual ~CDataTypeDouble();
    virtual size_t Sizeof() const {return size;}
    virtual CDataTypeDouble* clone() const; 
    virtual void Print(ostream& os) const;
    virtual bool Equals(const CType& a);
private:
    size_t size = 8;
};

bool CDataTypeDouble::Equals(const CType& a){
    return CType::Equals(a);
}

void CDataTypeDouble::Print(ostream& os) const{
    os << "double ";
}

CDataTypeDouble* CDataTypeDouble::clone() const{
    return new CDataTypeDouble(*this);
}

CDataTypeDouble::~CDataTypeDouble(){
   // cout << "destrukce double" << endl;
}

//--------------------------------------------------------------------------------
class CDataTypeEnum: public CType
{
public:
    CDataTypeEnum(){}
    virtual ~CDataTypeEnum();
    virtual size_t Sizeof() const{return size;}
    virtual CDataTypeEnum* clone() const; 
    CDataTypeEnum & Add (const string & x);
    virtual void Print(ostream& os) const;
    virtual bool Equals(const CType& a);

    vector<string> enums;
 private:
    set<string> duplicity;
    size_t size = 4;
};



bool CDataTypeEnum::Equals(const CType& a){
    if (typeid(a) == typeid(*this)){
        const CDataTypeEnum& other = static_cast<const CDataTypeEnum&>(a);
        if(other.enums == enums)
            return true;
    }
    return false;
}

 void CDataTypeEnum::Print(ostream& os) const{
     os << "enum {" << endl;
     for(auto i: enums){
         os << i;
         if(i!= enums.back())
            os <<"," << endl;
     }
     os << " }";
 }

CDataTypeEnum & CDataTypeEnum::Add (const string & x){
    if(duplicity.find(x) == duplicity.end()){
        duplicity.insert(x);
        enums.push_back(x);
    }else{
        string ex = "Duplicate enum value: ";
        ex += x;
        throw CSyntaxException(ex);
    }
        return *this;
}

CDataTypeEnum::~CDataTypeEnum(){
    //cout << "destrukce enum" << endl;
}

CDataTypeEnum* CDataTypeEnum::clone() const{
    return new CDataTypeEnum(*this);
}

//--------------------------------------------------------------------------------
class CDataTypeStruct: public CType
{
public:
    CDataTypeStruct(const CDataTypeStruct &);
    CDataTypeStruct(){ /*cout << "vznik nove struct" << endl;*/}
    virtual ~CDataTypeStruct();
    virtual size_t Sizeof() const;
    CDataTypeStruct& AddField (const string &, const CType &);
    virtual CDataTypeStruct* clone() const; 
    virtual void Print(ostream& os) const;
    virtual bool Equals(const CType& a);
    CType & Field(const string&) const;
    vector<pair<string,CType *> > fields;
    set<string> Stduplicity;
};

CType & CDataTypeStruct::Field(const string& name) const{
    for(auto i: fields)
        if(i.first == name)
            return *(i.second);
    
        string ex = "Unknown field: ";
        ex += name;
        throw CSyntaxException(ex);
}

bool CDataTypeStruct::Equals(const CType& a){
    if (typeid(a) != typeid(*this)){  
       //cout << "neni to struct" << endl;
        return false;
    }
    const CDataTypeStruct& other = static_cast<const CDataTypeStruct&>(a);
    
    if(fields.size() != other.fields.size()){
       //cout << "nestejna velikost fields" << endl;
        return false;
    }
    
    for(unsigned i = 0; i < fields.size(); i++){
        if(*(fields[i].second) != *(other.fields[i].second) ){
       //cout << "nestejny typ ve fields: " << *(fields[i].second) << " a " << *(other.fields[i].second) << endl;
            return false;
        }
    }
    return true;
}

void CDataTypeStruct::Print(ostream& os) const{
    os << "struct {" << endl;
    for(auto it:fields){
        it.second->Print(os);
        os << " " << it.first << ";" << endl;
    }
    os << "}" << endl;
}

CDataTypeStruct::CDataTypeStruct(const CDataTypeStruct & src){
    for(auto it:src.fields)
        fields.push_back(make_pair(it.first, it.second->clone()));
    Stduplicity = src.Stduplicity;
    
    
}
size_t CDataTypeStruct::Sizeof() const{ //dodelat
    size_t result = 0;
    for(auto i: fields)
        result += i.second->Sizeof();
    return result;
}

CDataTypeStruct* CDataTypeStruct::clone() const{
    return new CDataTypeStruct(*this);
}

 CDataTypeStruct& CDataTypeStruct::AddField ( const string & name, const CType & x){
    // cout << "pridavam do struct " << name << endl;
     if(Stduplicity.find(name) == Stduplicity.end()){
        Stduplicity.insert(name);
        fields.push_back(make_pair(name, x.clone()));
    }else{
        string ex = "Duplicate field: ";
        ex += name;
       //cout << ex << endl;
        throw CSyntaxException(ex);
    }
     
     
      return *this;
  }
 
 CDataTypeStruct::~CDataTypeStruct(){
     //cout << endl << "destrukce struct" << endl;
     //cout << "fields je velke " << fields.size() << endl;
     for(auto it: fields){
        // cout << it.first << " - ";
         delete(it.second);
         //cout << endl;
     }
 }

#ifndef __PROGTEST__
//-------------------------------------------------------------------------------------------------
static bool
whitespaceMatch (const string & out, const string & ref)
{
  const char *o = out.c_str ();
  const char *r = ref.c_str ();

  while (1)
    {
      while (isspace (*o))
	o++;
      while (isspace (*r))
	r++;
      if (!*r && !*o)
	return true;
      if (!*r || !*o || *r++ != *o++)
	return false;
    }
}

//-------------------------------------------------------------------------------------------------
template < typename _T > static bool
whitespaceMatch (const _T & x, const string & ref)
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch (oss.str (), ref);
}

//-------------------------------------------------------------------------------------------------
int
main (void)
{   
    
  CDataTypeStruct a =
    CDataTypeStruct ().AddField ("m_Length",
				 CDataTypeInt ()).AddField ("m_Status",
							    CDataTypeEnum().Add ("NEW").Add
							    ("FIXED").Add
							    ("BROKEN").Add
							    ("DEAD")).AddField
    ("m_Ratio", CDataTypeDouble ()); 
  
 // a.fields[1].second->Print(cout);
  a.Print(cout);

  CDataTypeStruct b =
    CDataTypeStruct ().AddField ("m_Length",
				 CDataTypeInt ()).AddField ("m_Status",
							    CDataTypeEnum
							    ().Add ("NEW").Add
							    ("FIXED").Add
							    ("BROKEN").Add
							    ("READY")).AddField
    ("m_Ratio", CDataTypeDouble ());

  CDataTypeStruct c =
    CDataTypeStruct ().AddField ("m_First",
				 CDataTypeInt ()).AddField ("m_Second",
							    CDataTypeEnum
							    ().Add ("NEW").Add
							    ("FIXED").Add
							    ("BROKEN").Add
							    ("DEAD")).AddField
    ("m_Third", CDataTypeDouble ());

  CDataTypeStruct d =
    CDataTypeStruct ().AddField ("m_Length",
				 CDataTypeInt ()).AddField ("m_Status",
							    CDataTypeEnum
							    ().Add ("NEW").Add
							    ("FIXED").Add
							    ("BROKEN").Add
							    ("DEAD")).AddField
    ("m_Ratio", CDataTypeInt ());

  assert (whitespaceMatch (a, "struct\n"
			   "{\n"
			   "  int m_Length;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    DEAD\n"
			   "  } m_Status;\n" "  double m_Ratio;\n" "}"));

  assert (whitespaceMatch (b, "struct\n"
			   "{\n"
			   "  int m_Length;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    READY\n"
			   "  } m_Status;\n" "  double m_Ratio;\n" "}"));

  assert (whitespaceMatch (c, "struct\n"
			   "{\n"
			   "  int m_First;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    DEAD\n"
			   "  } m_Second;\n" "  double m_Third;\n" "}"));

  assert (whitespaceMatch (d, "struct\n"
			   "{\n"
			   "  int m_Length;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    DEAD\n"
			   "  } m_Status;\n" "  int m_Ratio;\n" "}"));
  
 // CDataTypeInt jedna, dva;
  //bool res = (jedna == dva);
  //cout << res;
  //CDataTypeEnum mujEnum, tvujEnum;
  //mujEnum.Add("HH").Add("EE");
  //tvujEnum.Add("HH").Add("EE");
   //res = (mujEnum != tvujEnum);
    //cout << res;
  

  assert (a != b);
  assert (a == c);
  assert (a != d);
  assert (a != CDataTypeInt ());
  assert (whitespaceMatch (a.Field ("m_Status"), "enum\n"
			   "{\n"
			   "  NEW,\n"
			   "  FIXED,\n" "  BROKEN,\n" "  DEAD\n" "}"));

  b.AddField ("m_Other", CDataTypeDouble ());

  a.AddField ("m_Sum", CDataTypeInt ());

  assert (whitespaceMatch (a, "struct\n"
			   "{\n"
			   "  int m_Length;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    DEAD\n"
			   "  } m_Status;\n"
			   "  double m_Ratio;\n" "  int m_Sum;\n" "}"));

  assert (whitespaceMatch (b, "struct\n"
			   "{\n"
			   "  int m_Length;\n"
			   "  enum\n"
			   "  {\n"
			   "    NEW,\n"
			   "    FIXED,\n"
			   "    BROKEN,\n"
			   "    READY\n"
			   "  } m_Status;\n"
			   "  double m_Ratio;\n" "  double m_Other;\n" "}"));
 //cout << a.Sizeof() << endl;
  assert (a.Sizeof () == 20);
  assert (b.Sizeof () == 24);
  try
  {
    a.AddField ("m_Status", CDataTypeInt ());
    assert ("AddField: missing exception!" == NULL);
  }
  catch (const exception & e)
  {
    assert (string (e.what ()) == "Duplicate field: m_Status");
  }

  try
  {
    cout << a.Field ("m_Fail") << endl;
    assert ("Field: missing exception!" == NULL);
  }
  catch (const exception & e)
  {
    assert (string (e.what ()) == "Unknown field: m_Fail");
  }

  try
  {
    CDataTypeEnum en;
    en.Add ("FIRST").Add ("SECOND").Add ("FIRST");
    assert ("Add: missing exception!" == NULL);
  }
  catch (const exception & e)
  {
    assert (string (e.what ()) == "Duplicate enum value: FIRST");
  }
     
  return 0;
}
#endif /* __PROGTEST__ */