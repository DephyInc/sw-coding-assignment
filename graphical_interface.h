#include <set>
#include <map>

#include "datafile.h"
#include "utils.h"


using namespace std;


class Graphic {
    public:
        static inline int FCOLOR_RED = 31;
        static inline int FCOLOR_GREEN = 32;
        static inline int FCOLOR_YELLOW = 33;
        static inline int FCOLOR_UNKNOWN = 34;
        static inline int FCOLOR_GRAY = 39;
        static inline string OPEN_SQUARE = "\u25A1";
        static inline string FILLED_SQUARE = "\u25A3";
};


class GraphicSignature {
    public:
        string name;
        int colorCode = -1;
};


class GraphicalInterface {
    private:
        DataFile my_df;
        char nextFileColor = 'A';

        void printShape(string shape, int fc, int bc) {
            printf("\033[0;%dm", fc);
            printf("%s ", shape.c_str());
            printf("\033[0m");
        }

        void printStartFile(int step) {
            printf("\033[0m%d{", step);
        }

        void printEndFile() {
            printf("\033[0m}");
        }

    public:
        GraphicalInterface(DataFile df): my_df(df) {
            
        }

        void render() {

            printf("\nvEPROM File System\n");
            printf("File: %s\n\n", my_df.getFileName().c_str());
            

            vector<DataRepresentationAggDTO*> output = my_df.getBytesDTO();

            map<string, int> blockSignature;

            DataRepresentationAggDTO* prev_dto = NULL;
            set<int> activeFiles;

            for(const auto &el :output) {
                DataRepresentationAggDTO* dto = el;
                int step = dto->step;
                int freeBytes = dto->freeBytes;
                bool isCompletelyFree = dto->isCompletelyFree;
                vector<FileRepresentationDTO*> files = dto->files;

                string signature = "";
                int count = 0;
                for(const auto &file :files) {
                    signature += file->fileName;
                    if(count < files.size() - 1) {
                        signature += ", ";
                    }
                    count++;
                }
                
                // new signature
                if(signature.size() > 0 && blockSignature.count(signature) == 0) {
                    blockSignature[signature] = ++nextFileColor;
                }

                string shape = !isCompletelyFree ? Graphic::FILLED_SQUARE : Graphic::OPEN_SQUARE;

                if(dto->isSysSpc) {
                    printShape(shape, Graphic::FCOLOR_RED, 0);
                }
                else if(dto->isCtlSpc) {
                    printShape(shape, Graphic::FCOLOR_YELLOW, 0);
                }
                else if(dto->isUsrSpc) {
                    if(blockSignature.count(signature) > 0) {
                        shape = blockSignature[signature];
                    }
                    printShape(shape, Graphic::FCOLOR_GREEN, 0);
                }

                prev_dto = dto;
            }
            printf("\n");

            printf("\n\n_________Legend________\n\n");

            printf("Each block represents %d bytes.\n\n", my_df.getAggBytesSpan());

            printShape(Graphic::FILLED_SQUARE, 0, 0);
            printf(" - Occupied Space\n");
            printShape(Graphic::OPEN_SQUARE, 0, 0);
            printf(" - Empty Space\n");
            printShape(Graphic::OPEN_SQUARE, Graphic::FCOLOR_GREEN, 0);
            printf(" - User Space\n");
            printShape(Graphic::OPEN_SQUARE, Graphic::FCOLOR_YELLOW, 0);
            printf(" - Control Space (internal)\n");
            printShape(Graphic::OPEN_SQUARE, Graphic::FCOLOR_RED, 0);
            printf(" - System Space (internal)\n");

            printf("\n\n_________Files_________\n\n");

            if(blockSignature.size() > 0) {
                string icon = Graphic::OPEN_SQUARE;
                for(auto const& [key, val] :blockSignature) {
                    icon = blockSignature[key];
                    printShape(icon, Graphic::FCOLOR_GREEN, 0);
                    printf("-> %s\n", key.c_str());
                }
            }
            else {
                cout << Localization::NO_FILES_FOUND_MSG << endl;
            }

            cout << endl << endl;
        }
};