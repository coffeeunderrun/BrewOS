typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard *guard)
{
    return *reinterpret_cast<char *>(guard) == 0;
}

extern "C" void __cxa_guard_release(__guard *guard)
{
    *reinterpret_cast<char *>(guard) = 1;
}

extern "C" void __cxa_guard_abort(__guard *)
{
}

extern "C" void __cxa_pure_virtual()
{
}
