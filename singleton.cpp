#include"singleton.hpp"


int main()
{
   FILE* file;
   Singleton<FILE*> *s1=Singleton<FILE*>::Instance(file);
   Singleton<FILE*> *s2=Singleton<FILE*>::Instance(file);
   Singleton<FILE*> *s3=Singleton<FILE*>::Instance(file);
   assert(s1==s2);
   assert(s3==s2);
   cout<<"singelton1= "<<s1<<"\n";
   cout<<"singelton2= "<<s2<<"\n";
   cout<<"singelton3= "<<s3<<"\n";
   cout<<"the same address!\n";
   cout<<"Destroy:\n";
   s1->Destroy();
   Singleton<FILE*> *s4=Singleton<FILE*>::Instance(file);
   cout<<"singelton4= "<<s4<<"\n";
   assert(s1!=s4);
   assert(s2!=s4);
   assert(s3!=s4);
   cout<<"diffrent address!\n";

}
