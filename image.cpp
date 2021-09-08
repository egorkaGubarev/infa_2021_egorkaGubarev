#include <fstream>
#include <iostream>
#include <vector>

typedef unsigned char uc;
typedef unsigned int ui;

class Image
{
    public:
        Image(): depth_{0}, height_{0}, width_{0}, bitmap_{0}
        {

        }

        Image (ui width, ui height, uc depth)
        {
            create_black_image(width, height, depth);
        }

        Image (uc* raw, ui raw_amount, ui width, ui height, uc depth)
        {
            create_black_image(width, height, depth);
            uc depth_current = 0;
            ui height_current = 0;
            ui width_current = 0;
            for (ui index = 0; index < raw_amount; index ++) {
                uc bit = raw[index];
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

        Image (Image const& other)
        {
            depth_ = other.depth_;
            height_ = other.height_;
            width_ = other.width_;
            bitmap_ = other.bitmap_;

        }

        void crop(ui x_i, ui y_i, ui x_f, ui y_f)
        {
            for (ui string = y_i; string <= y_f; string ++) {
                for (ui column = x_i; column <= x_f; column ++) {
                    for (uc layer = 0; layer < depth_; layer ++) {
                        bitmap_[column][string][layer] = 0;
                    }
                }
            }
        }

        ui get_width()
        {
            return width_;
        }

        ui get_height()
        {
            return height_;
        }

        ui get_depth()
        {
            return depth_;
        }

        uc at(ui x, ui y, uc d)
        {
            uc bit = bitmap_[x][y][d];
            return bit;
        }

        void to_gs()
        {
            for (ui string = 0; string < height_; string ++) {
                for (ui column = 0; column < width_; column ++) {
                    ui sum = 0;
                    for (ui layer = 0; layer < depth_; layer ++) {
                        sum += bitmap_[column][string][layer];
                    }
                    ui average = sum / depth_;
                    for (ui layer = 0; layer < depth_; layer ++) {
                        bitmap_[column][string][layer] = average;
                    }
                }
            }
        }

        void print()
        {
            for (ui string = 0; string < height_; string ++) {
                for (ui column = 0; column < width_; column ++) {
                    ui intensity = 0;
                    for (ui layer = 0; layer < depth_; layer ++) {
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

        void to_bmp(std::string path)
        {
            std::ofstream file;
            file.open(path, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cout << "File couldn't be opened!" << '\n';
                return;
            }
            uc bmp_pad[3] = {0, 0, 0};
            ui width_bytes = width_ * 3;
            uc bytes_rest = width_bytes % 4;
            uc bytes_left = 4 - bytes_rest;
            uc amount_padding = bytes_left % 4;
            uc size_header_file = 14;
            uc size_header_information = 40;
            ui size_file = size_header_file + size_header_information + width_ * height_ * 3 + amount_padding * height_;
            uc header_file[size_header_file];
            // File type
            header_file[0] = 'B';
            header_file[1] = 'M';
            // File size
            header_file[2] = size_file;
            header_file[3] = size_file >> 8;
            header_file[4] = size_file >> 16;
            header_file[5] = size_file >> 24;
            // Reserve 1 (not used)
            header_file[6] = 0;
            header_file[7] = 0;
            // Reserve 2 (not used)
            header_file[8] = 0;
            header_file[9] = 0;
            // Pixel data offset
            header_file[10] = size_header_file + size_header_information;
            header_file[11] = 0;
            header_file[12] = 0;
            header_file[13] = 0;
            uc header_information[size_header_information];
            // Header size
            header_information[0] = size_header_information;
            header_information[1] = 0;
            header_information[2] = 0;
            header_information[3] = 0;
            // Image width
            header_information[4] = width_;
            header_information[5] = width_ >> 8;
            header_information[6] = width_ >> 16;
            header_information[7] = width_ >> 24;
            // Image height
            header_information[8] = height_;
            header_information[9] = height_ >> 8;
            header_information[10] = height_ >> 16;
            header_information[11] = height_ >> 24;
            // Planes
            header_information[12] = 1;
            header_information[13] = 0;
            // Bits per pixel (RGB)
            header_information[14] = 24;
            header_information[15] = 0;
            // Compression (no compression)
            header_information[16] = 0;
            header_information[17] = 0;
            header_information[18] = 0;
            header_information[19] = 0;
            // Image size (no compression)
            header_information[20] = 0;
            header_information[21] = 0;
            header_information[22] = 0;
            header_information[23] = 0;
            // X pixels per meter (not specified)
            header_information[24] = 0;
            header_information[25] = 0;
            header_information[26] = 0;
            header_information[27] = 0;
            // Y pixels per meter (not specified)
            header_information[28] = 0;
            header_information[29] = 0;
            header_information[30] = 0;
            header_information[31] = 0;
            // Total colours (colour palette not used)
            header_information[32] = 0;
            header_information[33] = 0;
            header_information[34] = 0;
            header_information[35] = 0;
            // Important colours (generally ignored)
            header_information[36] = 0;
            header_information[37] = 0;
            header_information[38] = 0;
            header_information[39] = 0;
            file.write(reinterpret_cast<char*>(header_file), size_header_file);
            file.write(reinterpret_cast<char*>(header_information), size_header_information);
            for(ui string = 0; string < height_; string ++){
                for (ui column = 0; column < width_; column ++){
                    uc red = static_cast<uc>(bitmap_[column][string][0]);
                    uc green = static_cast<uc>(bitmap_[column][string][1]);
                    uc blue = static_cast<uc>(bitmap_[column][string][2]);
                    uc colour[] = {blue, green, red};
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
        uc depth_;
        ui height_, width_;
        std::vector <std::vector <std::vector <uc>>> bitmap_;

        void create_black_image(ui width, ui height, uc depth)
        {
            if (depth == 1 || depth == 3){
                depth_ = depth;
            }else{
                std::cerr << "Depth must be 1 or 3! Result might be incorrect!" << '\n';
            }
            height_ = height;
            width_ = width;
            bitmap_.resize(width_);
            for (ui column = 0; column < width_; column ++) {
                bitmap_[column].resize(height_);
                for (ui string = 0; string < height_; string ++) {
                    bitmap_[column][string].resize(depth_);
                    for (uc layer = 0; layer < depth; layer ++) {
                        bitmap_[column][string][layer] = 0;
                    }
                }
            }
        }
};

int main()
{
    const uc intensity_max = 255;
    ui width = 640;
    ui height = 480;
    uc depth = 3;
    std::string path = "my_image.bmp";
    ui raw_amount = depth * height * width;
    uc* raw = new uc[raw_amount];
    for (ui bit = 0; bit < raw_amount; bit ++){
        uc component = (bit) % (intensity_max + 1);
        raw[bit] = component;
    }
    Image my_image(raw, raw_amount, width, height, depth);
    delete[] raw;
    my_image.to_bmp(path);
    return 0;
}
