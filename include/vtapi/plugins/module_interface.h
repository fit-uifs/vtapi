#pragma once


namespace vtapi {

class IMethodInterface
{
public:
	IMethodInterface() {}
	virtual ~IMethodInterface() {}

	virtual int dummy() = 0;
};

}
