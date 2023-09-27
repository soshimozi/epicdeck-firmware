#include <usb_names.h>

#define MANUFACTURER_NAME	{'E','p','i','D','e','c','k', ' ', 'H', 'u', 'b'}
#define MANUFACTURER_NAME_LEN 10
#define PRODUCT_NAME		{'E','p','i','D','e','c','k',' ','H','u','b', ' ', 'D', 'e', 'v', 'i', 'c', 'e'}
#define PRODUCT_NAME_LEN	18

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
  2 + MANUFACTURER_NAME_LEN * 2,
  3,
  MANUFACTURER_NAME
};
struct usb_string_descriptor_struct usb_string_product_name = {
  2 + PRODUCT_NAME_LEN * 2,
  3,
  PRODUCT_NAME
};

