Setup and Building Instructions

Files to Update with Your Absolute Path

Update the following C++ files with the full path to your Project_ALL directory:

src/my_functions/user_management.cpp

const string USERS_JSON_FILE_PATH = "C:/Your/Full/Path/To/Project_ALL/data/users.json";

src/my_functions/charity_management.cpp

string filePath = "C:/Your/Full/Path/To/Project_ALL/data/charities.json";

src/my_functions/donation_management.cpp

string filePath = "C:/Your/Full/Path/To/Project_ALL/data/donations.json";

src/my_functions/panels.cpp

string project_root_path = "C:/Your/Full/Path/To/Project_ALL/";

src/logger.cpp

const string PROJECT_ROOT_FOR_JSON_LOG_PATHS = "C:/Your/Full/Path/To/Project_ALL/";





Project_ALL
├── CMakeLists.txt
├── data
│   ├── users.json
│   ├── donations.json
│   ├── charities.json
│   └── unilogo.png
├── MY_Pdfs
├── output
│   └── logs.json
├── src
│   ├── main.cpp
│   ├── MY_hashingthingys
│   │   ├── bcrypt.h
│   │   └── bcrypt.cpp
│   ├── my_functions
│   │   ├── user_management.cpp
│   │   ├── charity_management.cpp
│   │   ├── donation_management.cpp
│   │   ├── panels.cpp
│   │   ├── exporttopdf.cpp
│   │   └── utils.cpp
│   ├── nlohman
│   │   └── json.hpp
│   ├── structsandfunctions
│   │   ├── structures.h
│   │   ├── functions.h
│   │   ├── exporttopdf.h
│   │   └── log_thingy.h
│   └── logger.cpp
└── cmake-build-debug