@echo off

SET solution_dir=%1
SET project_name=%2
SET file_pre=%3
SET file_ext=%4
SET platform=%5
SET config=%6
SET build_dir=%solution_dir%build\%project_name%\%platform%\%config%\
xcopy /y %build_dir%%file_pre%.lib %solution_dir%Lib\%platform%\%config%\
if "%file_ext%"==".dll" (
	xcopy /y %build_dir%%file_pre%.pdb %solution_dir%Lib\%platform%\%config%\
	xcopy /y %build_dir%%file_pre%%file_ext% %solution_dir%bin\%config%\
	)
)
if "%file_ext%"==".exe" (
	xcopy /y %build_dir%%file_pre%.pdb %solution_dir%Lib\%platform%\%config%\
	xcopy /y %build_dir%%file_pre%%file_ext% %solution_dir%\bin\%config%\
)
if "%project_name%"=="CSJAudioVideoTool" (
	xcopy /e /y %solution_dir%\resources\ %solution_dir%\bin\%config%\resources\
)



