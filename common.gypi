{
	'variables': {
		'device%': 'any',      # 'raspi' for Raspberry PI
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
				
				'msvs_settings': {
					'VCCLCompilerTool': {
						'ProgramDataBaseFileName':'$(OutDir)\\$(ProjectName).pdb',
					} # VCCLCompilerTool
				} # msvs_settings
			}],

			['arch=="x64"', {
				'msvs_configuration_platform': 'x64'				
			}],
			
			['OS!="win"', { 
            'sources/': [['exclude', '_win\\.cc$|_win\\.h$']]        
         }],

         # for Raspberry PI
         ['device=="raspi"', {
            'defines': ['_RASPI'],
         }],

         # anything other than Raspberry PI
         ['device!="raspi"', {
            'sources/': [['exclude', '_raspi\\.cc$|_raspi\\.h$|_egl\\.cc$|_egl\\.cc$']],
         }],
			
		], # conditions		
	
		'configurations': {
			'Release': {
								
				'conditions': [
					['OS=="win"', {
						'defines': ['_WIN32'],
						
						'msvs_settings': {
							'VCCLCompilerTool': {
								'Optimization': '2',
								'InlineFunctionExpansion': '2',
								'EnableIntrinsicFunctions': 'true',
								'FavorSizeOrSpeed': '0',
								'StringPooling': 'true',
								'RuntimeLibrary': '0'  # /MT
							}, # VCCLCompilerTool
							'VCLinkerTool': {					
								'LinkIncremental': '1',
								'OptimizeReferences': '2',
								'EnableCOMDATFolding': '2',
								'AdditionalLibraryDirectories': '..\\deps\\lib\\win32\\<(arch)\\Release'
							}, # VCLinkerTool
						} # msvs_settings
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
						
						'msvs_settings': {
							'VCCLCompilerTool': {
								'Optimization': '0',
								'RuntimeLibrary': '1',  # /MTd
							}, # VCCLCompilerTool
							'VCLinkerTool': {					
								'GenerateDebugInformation': 'true',
								'LinkIncremental': '2',
								'AdditionalLibraryDirectories': '..\\deps\\lib\\win32\\<(arch)\\Debug'
							}, # VCLinkerTool
						} # msvs_settings
					}]
										
				] # conditions
					
			} # Debug
			
		}, #configurations
	}	
}
