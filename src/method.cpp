/**
 * @file
 * @brief   Methods of Method class
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#include <common/vtapi_global.h>
#include <data/vtapi_method.h>

using namespace std;

namespace vtapi {


Method::Method(const KeyValues& orig, const string& name)
    : KeyValues(orig)
{
    thisClass = "Method";
    
    select = new Select(orig);
    select->from("public.methods", "*");
    if (!name.empty()) select->whereString("mtname", name);
    
    this->method = name;
}

Method::~Method()
{}


bool Method::next()
{
    if (KeyValues::next()) {
        this->method = this->getName();
        return true;
    }
    else {
        return false;
    }
}

int Method::getId()
{
    return this->getInt("id");
}

string Method::getName()
{
    return this->getString("mtname");
}


TKeys Method::getMethodKeys()
{
    KeyValues kv(*this);
    kv.select = new Select(*this);
    kv.select->from("public.methods_keys", "keyname");
    kv.select->from("public.methods_keys", "typname::text");
    kv.select->from("public.methods_keys", "inout");
    kv.select->whereString("mtname", this->method);

    TKeys keys;
    while (kv.next()) {
        keys.push_back(
            TKey(
            kv.getString("typname"),
            kv.getString("keyname"),
            0,
            kv.getString("inout")));
    }

    return keys;
}

void Method::printMethodKeys()
{
    KeyValues kv(*this);
    kv.select = new Select(*this);
    kv.select->from("public.methods_keys", "keyname");
    kv.select->from("public.methods_keys", "typname::text");
    kv.select->from("public.methods_keys", "inout");
    kv.select->from("public.methods_keys", "default_num");
    kv.select->from("public.methods_keys", "default_str");
    kv.select->whereString("mtname", this->method);

    if (kv.next()) {
        kv.printAll();
    }
}

bool Method::preUpdate()
{
    bool ret = KeyValues::preUpdate("public.methods");
    if (ret) {
        ret &= update->whereString("mtname", this->method);
    }

    return ret;
}

}
