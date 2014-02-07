{
   'includes': ['./common.gypi'],
   
   'targets': [
      
      {
         'target_name': 'core',
         'type': 'static_library',
            
         'sources': [
            './src/core/core.h',
            './src/core/convert.h',
            './src/core/convert.cc',
            './src/core/file.h',
            './src/core/file.cc'
            './src/core/stopwatch.h',
            './src/core/stopwatch.cc',
            './src/core/trace.h',
            './src/core/trace.cc',
            './src/core/uuid.h',
            './src/core/uuid.cc'
         ],

         'include_dirs+': [
         ],
      },
      
      {
         'target_name': 'net',
         'type': 'static_library',

         'include_dirs+': [
            './src/core',
         ],
            
         'sources': [
            './src/net/net.h',
            './src/net/mesh.h',
            './src/net/mesh.cc'
         ],		
      },

      {
         'target_name': 'gfx',
         'type': 'static_library',
         
         'include_dirs+': [
            './src/core',
         ],
         
         'defines': ['FREEIMAGE_LIB'],
            
         'sources': [
            './src/gfx/gfx.h',
            './src/gfx/gfx.cc',
            './src/gfx/context.h',
            './src/gfx/context.cc',
                './src/gfx/context_egl.cc',						
            './src/gfx/context_egl.h',
            './src/gfx/image.h',
            './src/gfx/image.cc',
            './src/gfx/matrix.h',
            './src/gfx/shader.h',
            './src/gfx/shader.cc',				
         ],

         'conditions': [
            ['OS=="win"', {
               'sources': [
                  './src/gfx/context_win.h',
                  './src/gfx/context_win.cc',
                   './src/gfx/glew/glew.c',					
                  './src/gfx/glew/GL/glew.h',
                    './src/gfx/glew/GL/wglew.h'
               ],
               
               'sources/': [['exclude', '_egl\\.cc$|_egl\\.h$']],
               
               'defines': ['GLEW_STATIC','GLEW_NO_GLU'],
               'include_dirs+': [
                  './src/gfx/glew/',
                  './deps/include',						
               ]
            }],	

                ['OS=="linux"', {
                'include_dirs+': [
                   '/opt/vc/include',
                  '/opt/vc/include/interface/vcos/pthreads',
               ]
            }]					
            
         ] # conditions
      },
      
      {
         'target_name': 'client',
         'type': 'executable',
         
         'dependencies': [
            ':core',
         ],
         
         'include_dirs+': [
            './src/core',
         ],
            
         'sources': [
            './src/version.h',
            './src/client/client.h',				
            './src/client/main.cc',
         ],
         
         'conditions': [
            ['OS=="win"', {
               
               'link_settings':  {
               'libraries': [ '-lkernel32.lib;psapi.lib;advapi32.lib;iphlpapi.lib;ws2_32.lib;winmm.lib;libuv.lib' ]
               },
               
            }],
            
            ['OS=="linux"', {
                
               'sources': [						
               ],
            
               'include_dirs+': [
                   '/opt/vc/include',
                  '/opt/vc/include/interface/vcos/pthreads',
               ],
               
               'link_settings': {
                   'ldflags': ['-L/opt/vc/lib -L../deps/lib/rpi/Release'],						
                   'libraries': [ '-luv -lpthread -lrt' ]
               }
            }]
            
         ] # conditions		
      },
   
      {
         'target_name': 'reno',
         'type': 'executable',
         
         'dependencies': [
            ':core',
            ':gfx',
            ':net'
         ],
         
         'include_dirs+': [
            './src/core',
            './src/net',
            './src/gfx'
         ],
            
         'sources': [
            './src/version.h',
            './src/server/channel.h',
            './src/server/channel.cc',
            './src/server/channel_host.h',
            './src/server/channel_host.cc',				
            './src/server/server.h',				
            './src/server/server.cc',				
            './src/server/main.cc',
            './src/server/window_win.h',
            './src/server/window_win.cc',
            './src/server/window_rpi.h',
            './src/server/window_rpi.cc'
         ],
         
         'conditions': [
            ['OS=="win"', {
                              
               'link_settings':  {
                  'libraries': [ '-lkernel32.lib;psapi.lib;user32.lib;gdi32.lib;advapi32.lib;iphlpapi.lib;opengl32.lib;ws2_32.lib;winmm.lib;FreeImage.lib;libuv.lib' ]
               },
               
            }],
            
            ['OS=="linux"', {
                
               'sources': [						
               ],
            
               'include_dirs+': [
                   '/opt/vc/include',
                   '/opt/vc/include/interface/vcos/pthreads',
                  '/opt/vc/include/interface/vmcs_host/linux'
               ],
               
               'link_settings': {
                   'ldflags': ['-L/opt/vc/lib -L../deps/lib/linux/Release'],						
                   'libraries': [ '-lfreeimage -lbcm_host -lEGL -lGLESv2 -luv' ]
               }
            }]
            
         ] # conditions
      
      },

   ]	
}
