#pragma once

/* This function will be run when the destructor is invoked.
* The function will be provided with a pointer to the instance about to be destroyed, 
* however this pointer will henceforth be invalid.
*/ 
template<typename T>
using OnDestructionCallback = std::function<void(T*)>;
