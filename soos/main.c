#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include "fs/sdfs.h"

#define hangmacro() \
({\
    puts("Press a key to exit...");\
    while(aptMainLoop())\
    {\
        hidScanInput();\
        if(hidKeysDown())\
        {\
            goto killswitch;\
        }\
        gspWaitForVBlank();\
    }\
})

#define min(a,b) ((a)<(b)?(a):(b))
#define BYTE3_TO_UINT(bp) \
     (((u32)(bp)[0] << 16) & 0x00FF0000) | \
     (((u32)(bp)[1] << 8) & 0x0000FF00) | \
     ((u32)(bp)[2] & 0x000000FF)

#define BYTE2_TO_UINT(bp) \
    (((u32)(bp)[0] << 8) & 0xFF00) | \
    ((u32)(bp)[1] & 0x00FF)

#define listdir() \
({\
    memset(&open, 0, sizeof(open)); \
    sdfs_scandir("", open.files, NULL, &open.cnt, (char*)-1); \
    sdfs_scandir("", open.files, open.extt, &open.cnt, (char*)-2); \
})

int main()
{
  // =====[PROGINIT]=====
  
  gfxInit(GSP_RGBA8_OES, GSP_RGBA8_OES, false);
  
  gfxSetDoubleBuffering(GFX_TOP, false);
  gfxSetDoubleBuffering(GFX_BOTTOM, false);
  
  // =====[VARS]=====
  
  u32 kDown;
  u32 kHeld;
  u32 kUp;
  u32* fbBottom;
  PrintConsole console;
  
  u64 slow = 0;
  int cx, cy;
  int redrva = 1;
  
  struct GuiOpen
  {
      u32 sel;
      u32 scroll;
      u32 cnt;
      char files[64][262];
      char extt[64][16];
  } open;
  memset(&open, 0, sizeof(open));
  
  char seldir[262];
  seldir[0] = '/';
  seldir[1] = 0;
  
  char rompath[262];
  memset(rompath, 0, sizeof(rompath));
  char ipspath[262];
  memset(ipspath, 0, sizeof(ipspath));
  
  // =====[PREINIT]=====
  
  consoleInit(GFX_TOP, &console);
  console.cursorY = 2;
  puts("[MAIN] Console attached");
  
  fbBottom = (u32*)gfxGetFramebuffer(GFX_BOTTOM, 0, NULL, NULL);
  
  puts("[MAIN] Initializing SDFS");
  sdfs_init(seldir);
  
  puts("[MAIN] Initializing game engine");
  listdir();
  
  //TODO implement
  
  puts("[MAIN] Loading complete ^^");
  
  gspWaitForVBlank();
  gspWaitForVBlank();
  gspWaitForVBlank();
  
  // =====[RUN]=====
  
  while (aptMainLoop())
  {
    slow = svcGetSystemTick();
    gspWaitForVBlank();
    slow = svcGetSystemTick() - slow;
    
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();
    
    if (kHeld & KEY_SELECT)
    {
		break;
    }
    
    if(kDown & KEY_DOWN)
    {
        open.sel = (open.sel + 1) % open.cnt;
        redrva = 1;
    }
    if(kDown & KEY_UP)
    {
        if(!open.sel) open.sel = open.cnt - 1;
        else
            open.sel -= 1;
        redrva = 1;
    }
    if(kDown & KEY_LEFT)
    {
        if(open.sel < 5) open.sel = open.cnt - (5 - open.sel);
        else open.sel = open.sel - 5;
        redrva = 1;
    }
    if(kDown & KEY_RIGHT)
    {
        open.sel = (open.sel + 5) % open.cnt;
        redrva = 1;
    }

    if(kDown & KEY_A)
    {
        if(open.files[open.sel][0] == '/')
        {
            sdfs_free();
            if(*(u16*)seldir == (u16)'/') *seldir = 0;
            else *(seldir + strlen(seldir) - 1) = 0;
            strcat(seldir, open.files[open.sel]);
            strcat(seldir, "/");
            sdfs_init(seldir);
            listdir();
            redrva = 1;
        }
        else if(open.files[open.sel][0] && open.files[open.sel][0] != '/')
        {
            memset(rompath, 0, 262);
            strcat(rompath, seldir);
            strcat(rompath, open.files[open.sel]);
            redrva = 1;
        }
    }

    if(kDown & KEY_X)
    {
        if(open.files[open.sel][0] && !strcmp(".ips", open.extt[open.sel]))
        {
            memset(ipspath, 0, 262);
            strcat(ipspath, seldir);
            strcat(ipspath, open.files[open.sel]);
            redrva = 1;
        }
    }

    if(kDown & KEY_B)
    {
        if(*(u16*)seldir != (u16)'/')
        {
            char* end = seldir + strlen(seldir) - 1;
            while(*--end != '/');
            end++;
            memset(end, 0, seldir + strlen(seldir) - end);

            sdfs_free();
            sdfs_init(seldir);
            listdir();
            redrva = 1;
        }
    }

    if(redrva)
    {
        consoleClear();
        //puts("\e[0;1;37;7;42m\e[2K\e[4;31mREDThreeUp \e[32mby MarcusD");
        puts("\e[0m\e[1;37;7;41m\e[2K\e[4mREDThreeUp\e[24m \e[42mby MarcusD");
        do{fputs("\e[24;27;37;46m\e[2K> ",stdout);u32 slen=strlen(seldir);if(slen<48)puts(seldir);else printf("\e[40;1;30m[...]\e[27;37;46m%s\n",seldir+strlen(seldir)-44);}while(false);
        do{fputs("\e[27;37;46m\e[2KROM: ",stdout);u32 slen=strlen(rompath);if(slen<45)puts(rompath);else printf("\e[40;1;30m[...]\e[27;37;46m%s\n",rompath+strlen(rompath)-41);}while(false);
        do{fputs("\e[4;27;37;46m\e[2KIPS: ",stdout);u32 slen=strlen(ipspath);if(slen<45)puts(ipspath);else printf("\e[40;1;30m[...]\e[27;37;46m%s\n",ipspath+strlen(ipspath)-41);}while(false);
        puts("\e[0m");

        if(!open.cnt)
        {
            puts("\n\n     --- There are no files ---");
        }
        else
        {
            char* formats[] =
            /* dir_nosel     */{"\e[37m%s\n",
            /* dir_nosel_len */ "\e[37m%.44s\e[1;30m[...]\e[21m\n",
            /* fil_nosel     */ "\e[1;37m%.*s\e[21;36m%s\n",
            /* fil_nosel_len */ "\e[1;37m%.*s\e[30m[...]\e[21;36m%s\n",
            /* dir_sel       */ "%s\e[40m\n",
            /* dir_sel_len   */ "%.44s\e[1;7;40;37m[...]\e[21;27;40m\n",
            /* fil_sel       */ "%.*s\e[36m%s\e[40m\n",
            /* fil_sel_len   */ "%.*s\e[1;7;40;37m[...]\e[21;27;36m%s\e[40m\n"};

            int offs = open.sel > 10 ? (open.cnt > 21 ? (min(open.cnt - 21, open.sel - 10)) : 0) : 0;
            int to =  min(open.cnt, offs + 21);
            for(cx = offs; cx != to; cx++)
            {
                char* fi = open.files[cx + open.scroll];
                int filen = strlen(fi);
                char* ex = open.extt[cx + open.scroll];
                int exlen = strlen(ex);


                int ind = 0;
                ind |= filen > 49;
                ind |= (*fi != '/') << 1;
                if(cx == open.sel)
                {
                    ind |= 4;
                    fputs("\e[47;30m", stdout);
                }



                switch(ind & 3)
                {
                    case 0:
                    case 1: printf(formats[ind], fi);                    break;
                    case 2: printf(formats[ind], filen - exlen, fi, ex); break;
                    case 3: printf(formats[ind], 44 - exlen, fi, ex);    break;
                }
            }
        }

        console.cursorX = 1;
        console.cursorY = console.consoleHeight - 2;

        puts(" \e[31m[A] Set ROM \e[34m[X] Set IPS \e[33m[B] Up \e[1;30m[Select] Exit\e[0m");
        if(*rompath && *ipspath) fputs("     Press \e[1;30m[START]\e[0m to start patching here\e[0m", stdout);

        redrva = 0;
    }
    
    if((kDown & KEY_START) && *rompath && *ipspath)
    {
        consoleClear();
        SwkbdState swkbd;
        char textbuf[262];
        memset(textbuf, 0, sizeof(textbuf));
        swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, 262);
        char* initptr = ipspath + strlen(ipspath);
        while(*--initptr != '/');
        initptr++;
        strncpy(textbuf, initptr, strlen(initptr) - 4);
        initptr = rompath + strlen(rompath);
        while(*--initptr != '.' && initptr != rompath);
        if(*initptr == '.') strcat(textbuf, initptr);
        swkbdSetInitialText(&swkbd, textbuf);
        if(swkbdInputText(&swkbd, textbuf, 262 - strlen(seldir) - 3) == SWKBD_BUTTON_RIGHT)
        {
            char filepath[262];
            memset(filepath, 0, sizeof(filepath));
            strcpy(filepath, seldir);
            strcat(filepath, textbuf);
            
            printf("Output path: %s\n", filepath);

            FILE* f = fopen(filepath, "wb");
            FILE* fin = fopen(rompath, "rb");

            u8* buf = malloc(2048);
            
            puts("Copying contents");
            while(fread(buf, 1, 2048, fin) > 0) fwrite(buf, 1, 2048, f);
            fflush(f);
            fclose(f);
            fclose(fin);
            
            puts("Preparing for patching...");
            fin = fopen(ipspath, "rb");
            if(fread(buf, 1, 5, fin) != 5 || memcmp(buf, "PATCH", 5))
            {
                fclose(fin);
                puts("[ERROR] Not a valid patch file");
                goto nope;
            }

            f = fopen(filepath, "rb+");

            u32 offs, size, remaining, eof = 0;
            
            puts("Patching...");
            while(1)
            {
                fread(buf, 3, 1, fin);
                eof = !memcmp(buf, "EOF", 3);
                offs = BYTE3_TO_UINT(buf);

                if(fread(buf, 2, 1, fin)) { if(eof) puts("[WARNING] \"EOF\" bug detected in the patch file!"); }
                else break;
                size = BYTE2_TO_UINT(buf);

                if(!size)
                {
                    fread(buf, 1, 3, fin);
                    u16 rep = BYTE2_TO_UINT(buf);
                    char wat = buf[2];
                    
                    printf("RLE: %06X %04X %02X '%c'\n", offs, rep, wat, wat);
                    
                    fseek(f, offs, SEEK_SET);
                    while(rep--) fwrite(&wat, 1, 1, f);

                    continue;
                }
                printf("DAT: %06X %04X\n", offs, size);
                
                fseek(f, offs, SEEK_SET);
                remaining = size;
                while(remaining)
                {
                    int readsize = fread(buf, 1, min(2048, remaining), fin);
                    if(readsize <= 0)
                    {
                        puts("[ERROR] Error reading input file, corrupted patch");
                        fclose(f);
                        fclose(fin);
                        goto nope;
                    }

                    fwrite(buf, 1, readsize, f);
                    printf("- %8X\n", remaining);

                    remaining -= readsize;
                }
            }

            fflush(f);
            fclose(f);
            fclose(fin);
            puts("Successful patch! Press a key to update screen");
        }
        else
        {
            redrva = 1;
        }

    }
    
    nope:
    
    cx = console.cursorX;
    cy = console.cursorY;
    
    console.cursorX = console.consoleWidth - 16;
    console.cursorY = 0;
    printf("%016llX", slow);
    
    console.cursorX = cx;
    console.cursorY = cy;
    
    
    gfxFlushBuffers();
    gfxSwapBuffers();
  }

  // =====[END]=====
  
  killswitch:
  
  puts("Freeing sdfs");
  
  sdfs_free();
  gfxExit();

  return 0;
}
