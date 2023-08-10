/*
 * Terminal software for Pi Pico
 * USB keyboard input, VGA video output, communication with RC2014 via UART on GPIO 20 & 21
 * Shiela Dixon, https://peacockmedia.software
 *
 * much of what's in this main file is taken from the VGA textmode example
 * and the TinyUSB hid_app
 * picoterm.c handles the behaviour of the terminal and storing the text
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#include <stdio.h>

#include <stdlib.h>
#include "pico.h"
#include "pico/stdlib.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/multicore.h"
#include "pico/sync.h"
#include "font.h" // Looks in under comment for 40 columns version
#include "hardware/irq.h"
#include <stdint.h>


//#include "bsp/board.h"
//#include "tusb.h"

#include "../common/pmhid.h"

#ifndef _MAIN_H
#define _MAIN_H


#define MENU_CONFIG    0x01 // support several menu
#define MENU_CHARSET   0x02 // display current charset
#define MENU_HELP      0x03 // display the HELP menu
#define MENU_COMMAND   0x04 // Key-in interpreter command


static uint32_t start_time;

static void pico_key_down(int scancode, int keysym, int modifiers);
static void pico_key_up(int scancode, int keysym, int modifiers);

void select_graphic_font( uint8_t font_id );
void build_font( uint8_t font_id );
// void read_data_from_flash();
// void write_data_to_flash();
void render_on_core1();
void stop_core1();



/*
#define c_Black	0, 0, 0
#define c_Red	170, 0, 0
#define c_Green	0, 170, 0
#define c_Yellow	170, 85, 0
#define c_Blue	0, 0, 170
#define c_Magenta	170, 0, 170
#define c_Cyan	0, 170, 170
#define c_White	170, 170, 170
#define c_BrightBlack	85, 85, 85
#define c_BrightRed	255, 85, 85
#define c_BrightGreen	85, 255, 85
#define c_BrightYellow	255, 255, 85
#define c_BrightBlue	85, 85, 255
#define c_BrightMagenta	255, 85, 255
#define c_BrightCyan	85, 255, 255
#define c_BrightWhite	255, 255, 255
*/

#define	c_Black	(((0)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Red	(((0)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Green	(((0)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Yellow	(((0)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Blue	(((170)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Magenta	(((170)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_Cyan	(((170)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((0)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_White	(((170)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((170)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightBlack	(((85)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightRed	(((85)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightGreen	(((85)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightYellow	(((85)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightBlue	(((255)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightMagenta	(((255)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightCyan	(((255)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((85)<<PICO_SCANVIDEO_PIXEL_RSHIFT))
#define	c_BrightWhite	(((255)<<PICO_SCANVIDEO_PIXEL_BSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_GSHIFT)|((255)<<PICO_SCANVIDEO_PIXEL_RSHIFT))

#endif // _MAIN_H


