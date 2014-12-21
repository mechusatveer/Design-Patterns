#include<iostream>
#include<map>
using namespace std;

class Vehicle
{
   public:
   virtual void print() = 0;

};

class Bus : public Vehicle
{
   public:
   Bus()
   {
      cout<<"Bus";
   }
   void print()
   {
      cout<<"Bus";
   }
   static Vehicle* __stdcall Create()
   {
       return new Bus();
   }
};

class Jeep : public Vehicle
{
   public:
   void print()
   {
      cout<<"Jeep";
   }

   static Vehicle* __stdcall Create()
   {
       return new Jeep();
   }
};

class Train : public Vehicle
{
   public:
   void print()
   {
      cout<<"Train";
   }
   static Vehicle* __stdcall Create()
   {
       return new Train();
   }
};

typedef Vehicle* (__stdcall *CreateVehicleFn)(void);

class Factory
{
   private:
   map<string,CreateVehicleFn> mp;
   public:
   Factory()
   {
      mp["Bus"] = &Bus::Create;
      mp["Jeep"] = &Jeep::Create;
      mp["Train"] = &Train::Create;
   }
   CreateVehicleFn getVehicle(string str)
   {
      return mp[str];
   }
};

int main()
{
   Factory *f = new Factory();

   CreateVehicleFn fw = f->getVehicle("Bus");
   fw();

   return 0;
}
