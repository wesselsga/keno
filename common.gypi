{
	'variables': {
		'distrib%': 'any',      # 'raspi' for Raspberry PI
	},

	'target_defaults': {

      'defines': [
		   'ARCH=<(arch)',
			'PLATFORM=<(OS)'
      ],
	
		'conditions': [
		
			['OS=="linux"', {
				'cflags_cc': ['-std=c++11 -pthread']
			}],		
		
			['OS=="win"', {
				'sources/': [['exclude', '_linux\\.cc$|_linux\\.h$']],
				
				'msvs_configuration_attributes': {
					'OutputDirectory': '.\\build\\$(ConfigurationName)\\$(Platform)\\',
					'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)\\',
					'CharacterSet': '1',
				},
				
			}],

			['arch=="x64"', {
				'msvs_configuration_platform': 'x64'				
			}],
			
			['OS!="win"', { 
            'sources/': [['exclude', '_win\\.cc$|_win\\.h$']]        
         }],

         # for Raspberry PI
         ['distrib=="raspi"', {
            'defines': ['_RASPI'],
         }],

         # anything other than Raspberry PI
         ['distrib!="raspi"', {
            'sources/': [['exclude', '_raspi\\.cc$|_raspi\\.h$|_egl\\.cc$|_egl\\.cc$']],
         }],
			
		], # conditions		
	
		'configurations': {
			'Release': {
								
				'conditions': [
					['OS=="win"', {
						'defines': ['_WIN32'],
													
						'msbuild_settings': {
							'ClCompile': {
                        'Optimization': 'MaxSpeed',
								'InlineFunctionExpansion': 'AnySuitable',
								'IntrinsicFunctions': 'true',
								'FavorSizeOrSpeed': 'Neither',
								'StringPooling': 'true',
								'RuntimeLibrary': 'MultiThreaded',  # /MT
                        'ProgramDataBaseFileName':'$(OutDir)\\$(ProjectName).pdb',                        
                     },
                     
                     'Link': {
                        'LinkTimeCodeGeneration': 'UseLinkTimeCodeGeneration',
                        'OptimizeReferences': 'true',
								'EnableCOMDATFolding': 'true',
								'AdditionalLibraryDirectories': '.\\lib\\<(arch)\\release'								
							},

                     'ResourceCompile': {
                        'Culture': '0x03eb',
                        'ShowProgress': 'true',
                        'SuppressStartupBanner': 'true',
                        
                     },			
							
						}	# msbuild_settings

					}]
					
				] # conditions								
				
			}, # Release			
		
			'Debug': {
				'defines': [
					'_DEBUG',
				],
				
				'conditions': [
					['OS=="win"', {
						'defines': ['_WIN32'],
												
						'msbuild_settings': {
							'ClCompile': {
                        'Optimization': 'Disabled',
								'RuntimeLibrary': 'MultiThreadedDebug',
                        'ProgramDataBaseFileName':'$(OutDir)\\$(ProjectName).pdb',
                     },
                     
                     'Link': {
								'GenerateDebugInformation': 'true',
								'AdditionalLibraryDirectories': '.\\lib\\<(arch)\\debug',
								'ImageHasSafeExceptionHandlers': 'false'
							},

                     #'': {
							#	'LinkIncremental': 'false'
							#}					
							
						}	# msbuild_settings					
					}],
               
					['OS=="linux"', {
				      'cflags': ['-g']
               }]
										
				] # conditions
					
			} # Debug
			
		}, #configurations
	}	
}
