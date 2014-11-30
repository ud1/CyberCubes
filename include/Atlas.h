#ifndef ATLAS_H
#define ATLAS_H


struct Atlas {
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned char	 *pixel_data = nullptr;

    void make(int c);
    void put(int c, const unsigned char *data);
};

#endif // ATLAS_H
