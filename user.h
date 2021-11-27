struct stat;
struct rtcdate;
struct rect;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getch(void);
int setvideomode(int);
//Assignment calls
int setpixel(int,int,int); //hdc,x,y
int moveto(int,int,int); //hdc,x,y
int lineto(int,int,int); //hdc,x,y
int setpencolour(int,int,int,int); //index r g b
int selectpen(int, int); //hdc index
int fillrect(int, struct rect*); //hdc, pointer to a rect
int beginpaint(int); //hwnd (always 0)
int endpaint(int); //hdc value
int getHDC(int); //hdc to get

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
