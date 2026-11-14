#include <mach/kmod.h>
extern kmod_start_func_t _start, BlueToolFixup_kern_start;
extern kmod_stop_func_t _stop, BlueToolFixup_kern_stop;
KMOD_EXPLICIT_DECL(as.acidanthera.BlueToolFixup, "2.7.3", _start, _stop)
kmod_start_func_t *_realmain = BlueToolFixup_kern_start;
kmod_stop_func_t *_antimain = BlueToolFixup_kern_stop;
