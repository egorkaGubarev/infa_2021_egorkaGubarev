#include <fstream>
#include <iostream>
#include <vector>

typedef unsigned char uchar;
typedef unsigned int uint;

class Image
{
    public:
        Image(): Image(0, 0, 0)
        {

        }

        Image (uint width, uint height, uchar depth)
        {
            create_black_image(width, height, depth);
        }

        Image (uchar* raw, uint raw_amount, uint width, uint height, uchar depth)
        {
            create_black_image(width, height, depth);
            uchar depth_current = 0;
            uint height_current = 0;
            uint width_current = 0;
            for (uint index = 0; index < raw_amount; index ++) {
                uchar bit = raw[index];
                bitmap_[width_current][height_current][depth_current] = bit;
                depth_current ++;
                if (depth_current == depth_) {
                    depth_current = 0;
                    width_current ++;
                    if (width_current == width_) {
                        width_current = 0;
                        height_current ++;
                    }
                }
            }
        }

        void crop(uint x_i, uint y_i, uint x_f, uint y_f)
        {
            for (uint string = y_i; string <= y_f; string ++) {
                for (uint column = x_i; column <= x_f; column ++) {
                    for (uchar layer = 0; layer < depth_; layer ++) {
                        bitmap_[column][string][layer] = 0;
                    }
                }
            }
        }

        uint get_width() const
        {
            return width_;
        }

        uint get_height() const
        {
            return height_;
        }

        uint get_depth() const
        {
            return depth_;
        }

        uchar at(uint x, uint y, uchar d) const
        {
            uchar bit = bitmap_[x][y][d];
            return bit;
        }

        void to_gs()
        {
            for (uint string = 0; string < height_; string ++) {
                for (uint column = 0; column < width_; column ++) {
                    uint sum = 0;
                    for (uint layer = 0; layer < depth_; layer ++) {
                        sum += bitmap_[column][string][layer];
                    }
                    uint average = sum / depth_;
                    for (uint layer = 0; layer < depth_; layer ++) {
                        bitmap_[column][string][layer] = average;
                    }
                }
            }
        }

        void print() const
        {
            for (uint string = 0; string < height_; string ++) {
                for (uint column = 0; column < width_; column ++) {
                    uint intensity = 0;
                    for (uint layer = 0; layer < depth_; layer ++) {
                        intensity += bitmap_[column][string][layer];
                    }
                    std::cout << intensity << ' ';
                }
                std::cout << '\n';
            }
        }

        void clear()
        {
            create_black_image(0, 0, 0);
        }

        Image operator = (Image const& other)
        {
            depth_ = other.depth_;
            height_ = other.height_;
            width_ = other.width_;
            bitmap_ = other.bitmap_;
            return *this;
        }

        void to_bmp(std::string path) const
        {
            std::ofstream file;
            file.open(path, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cout << "File couldn't be opened!" << '\n';
                return;
            }
            uchar bmp_pad[3] = {0, 0, 0};
            uint width_bytes = width_ * 3;
            uchar bytes_rest = width_bytes % 4;
            uchar bytes_left = 4 - bytes_rest;
            uchar amount_padding = bytes_left % 4;
            uchar size_header_file = 14;
            uchar size_header_information = 40;
            uint size_file = size_header_file + size_header_information + width_ * height_ * 3 + amount_padding * height_;
            uchar header_file[size_header_file] = {0};
            // File type
            header_file[0] = 'B';
            header_file[1] = 'M';
            // File size
            *(reinterpret_cast<int*>(&header_file[2])) = size_file;
            // Pixel data offset
            header_file[10] = size_header_file + size_header_information;
            uchar header_information[size_header_information] = {0};
            // Header size
            header_information[0] = size_header_information;
            // Image width
            *(reinterpret_cast<int*>(&header_information[4])) = width_;
            // Image height
            *(reinterpret_cast<int*>(&header_information[8])) = height_;
            // Planes
            header_information[12] = 1;
            // Bits per pixel (RGB)
            header_information[14] = 24;
            file.write(reinterpret_cast<char*>(header_file), size_header_file);
            file.write(reinterpret_cast<char*>(header_information), size_header_information);
            for(uint string = 0; string < height_; string ++){
                for (uint column = 0; column < width_; column ++){
                    uchar red = static_cast<uchar>(bitmap_[column][string][0]);
                    uchar green = static_cast<uchar>(bitmap_[column][string][1]);
                    uchar blue = static_cast<uchar>(bitmap_[column][string][2]);
                    uchar colour[] = {blue, green, red};
                    file.write(reinterpret_cast<char*>(colour), 3);
                }
                file.write(reinterpret_cast<char*>(bmp_pad), amount_padding);
            }
            file.close();
            std::cout << "File created!" << '\n';
        }

        ~Image()
        {

        }

    private:
        uchar depth_;
        uint height_, width_;
        std::vector <std::vector <std::vector <uchar>>> bitmap_;

        void create_black_image(uint width, uint height, uchar depth)
        {
            if (depth == 1 || depth == 3){
                depth_ = depth;
            }else{
                std::cerr << "Depth must be 1 or 3! Result might be incorrect!" << '\n';
            }
            height_ = height;
            width_ = width;
            bitmap_.resize(width_);
            for (uint column = 0; column < width_; column ++) {
                bitmap_[column].resize(height_);
                for (uint string = 0; string < height_; string ++) {
                    bitmap_[column][string].resize(depth_);
                    for (uchar layer = 0; layer < depth; layer ++) {
                        bitmap_[column][string][layer] = 0;
                    }
                }
            }
        }
};

int main()
{
    const uchar intensity_max = 255;
    uint width = 640;
    uint height = 480;
    uchar depth = 3;
    std::string path = "my_image.bmp";
    uint raw_amount = depth * height * width;
    uchar* raw = new uchar[raw_amount];
    for (uint bit = 0; bit < raw_amount; bit ++){
        uchar component = (bit) % (intensity_max + 1);
        raw[bit] = component;
    }
    Image my_image(raw, raw_amount, width, height, depth);
    delete[] raw;
    raw = NULL;
    my_image.to_bmp(path);
    return 0;
}
