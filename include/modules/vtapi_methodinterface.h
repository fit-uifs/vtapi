#pragma once

#include "../vtapi.h"

namespace vtapi {

class IMethodInterface
{
public:
	IMethodInterface() {}
	virtual ~IMethodInterface() {}

	virtual int dummy() = 0;
};

}