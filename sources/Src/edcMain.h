#ifdef __cplusplus
extern "C" {
    #include <stdarg.h>

#endif
	void edcScheluder();	
    void edcMain();
    void fatalLog(const char *fmt,...);
	extern void edcReceiveSystemEvent(const char*name,const char *data);
		
#ifdef __cplusplus
}
#endif
