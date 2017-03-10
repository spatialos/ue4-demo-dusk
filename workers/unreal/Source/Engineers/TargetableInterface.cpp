
#include "Engineers.h"
#include "TargetableInterface.h"


UTargetableInterface::UTargetableInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//This is required for compiling, would also let you know if somehow you called
//the base event/function rather than the over-rided version
//bool ITargetableInterface::IsTargetable() const
//{
//	return false;;
//}