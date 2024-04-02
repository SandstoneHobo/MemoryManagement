#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>

//variables to control pages and frames
const int page_size = 1024;
const int frame_size = 1024;
const int num_pages = 16;
const int num_frames = 8;

//helper function to find a number in a vector returns the index if its present or negative one if it is not
int numInVector(std::vector<int> vec, int num) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == num) {
            return i;
        }
    }
    return -1;
}

// page/frame table data structure
class pageFrameTable {
    //variable for the page table and keeping track of frames that have been used and in what order
    std::vector<int> page_table;
    std::vector<int> allocated_frames;
public:
    //constructor for the class to initialize both vectors and assigns the first 8 pages to the corresponding frame
    pageFrameTable() {
        for (int i = 0; i < num_pages; i++) {
            page_table.push_back(-1);
        }
        for (int i = 0; i < num_frames; i++) {
            page_table[i] = i;
            allocated_frames.push_back(i);
        }
    }

    int getFrameNumber(int page_number) {
        //returns the frame number associated with the page number in the page table
        return page_table[page_number];
    }

    //moves the front frame to the back of the list so we can have a FIFO algorithm
    void cycleFrame() {
        int start = allocated_frames.front();
        for (int i = 0; i < allocated_frames.size() - 1 ; i++) {
            allocated_frames[i] = allocated_frames[i + 1];
        }
        allocated_frames.pop_back();
        allocated_frames.push_back(start);
    }

    //allocates a new frame to the given page number
    void allocateFrame(int page_number) {
        int frame = allocated_frames.front();
        int idx = numInVector(page_table, frame);
        if (idx != -1) {
            page_table[page_number] = frame;
            page_table[idx] = -1;
            cycleFrame();
        }
    }
};

//helper function to add leading zeros for hexidecimal numbers during printing
std::string leadingZeros(int addr) {
    if (addr <= 0xF) {
        return "000";
    }
    else if (addr <= 0xFF) {
        return "00";
    }
    else if (addr <= 0xFFF) {
        return "0";
    }
    else {
        return "";
    }
}

int main()
{
    //variables for the frame table, secondary storage, and logical address
    pageFrameTable frame_table;
    std::vector<int> secondary_storage;
    int logical_address = 0;
    
    //loop to continue accepting logical addresses until -1 is recieved
    while(logical_address != -1) {
        //recieves user input as a hexadecimal number
        std::cout << "Enter a logical address(or -1 to quit): ";
        std::cin >> std::hex >> logical_address;

        //if -1 or an invalid input is recieved halts the program
        if (logical_address == -1 || logical_address > (page_size * num_pages)) {
            std::cout << "Given logical address was invalid." << std::endl;
            return 0;
        }

        //calculate page number and offset
        int page_number = logical_address / page_size;
        int offset = logical_address % page_size;

        //ensures the page number is valid
        if (page_number < num_pages) {
            //gets the frame number for the given page, and loads from secondary storage if not
            int frame_number = frame_table.getFrameNumber(page_number);
            if (frame_number == -1) {
                int storage_location = numInVector(secondary_storage, page_number);
                if (storage_location != -1) {
                    frame_table.allocateFrame(secondary_storage[storage_location]);
                }
                else {
                    //stores the missed frame in secondary storage and finds the next available frame
                    secondary_storage.push_back(page_number);
                    frame_table.allocateFrame(page_number);
                }
                //restarts the instruction
                frame_number = frame_table.getFrameNumber(page_number);
            }
            //calculate the physical address
            int physical_address = frame_number * frame_size + offset;

            //output the adresses and offsets
            std::cout << "Logical Address: 0x" << leadingZeros(logical_address) << std::hex << std::uppercase << logical_address;
            std::cout << " => Physical Address: 0x" << leadingZeros(physical_address) << std::hex << physical_address;
            std::cout << " => Page Number: 0x" << leadingZeros(page_number) << std::hex << page_number << " Offset: 0x" << std::hex << offset << std::endl;
       
           
        }
        else {
            std::cout << "Invalid Page Number: " << page_number << std::endl; // Prints an error message for invalid page number
        }
    }

    return 0;
    
}