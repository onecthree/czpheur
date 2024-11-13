# Zpheur
> The project are unstable, do it at your own risk
 
Zpheur is a web framework for PHP written in C delivered as extension. It includes router, middleware, dependency injection, dotenv loader, and more. Aims for a minimalist project structure without bloat-installed Composer package.

## Project Status
This project is under prototyping stuff and currently is not available for the stage of stable version. For the security concern is recommended to update the extension in your project every new commits. The testing of the extension are in my projects usage for now. So, if given such ```segmentation faults``` error in PHP land or other improvent/security needed, I usually just do debugging and fixes.

## Installation
> Extension development is on Ubuntu 20.04; other platform are not tested yet

Requirement:
- PHP 8.2.0 or greater
- openssl
- pcre2

For installing the library:
```bash
# Ubuntu
$ sudo apt install libssl-dev libpcre2-dev -y
```
The extension itself needs other extensions to run each feature (optional):
```bash
# Zpheur Voile (MongoDB ODM)
$ sudo apt install php8.x-mongodb -y
```
Clone the repository:
```bash
$ git clone https://github.com/zpheur/czpheur
```
Compile the extension:
```bash
$ cd czpheur
$ phpize
$ ./configure
$ make
```
Install the module:
```bash
$ make install
# Or you may need sudo privilege
$ sudo make install
```
While ```$ make install``` only moves the shared object ```zpheur.so``` to the PHP extension directory from the ```php-config``` directive, you may need to manually install ```zpheur.ini``` in the PHP configuration directory.

If PHP was installed using the apt package manager via ```ppa:ondrej/php```, the configuration directory is usually located at ```/etc/php/8.x```.\
\
For the configuration:
```bash
# Under the czpheur clone directory
$ sudo mv zpheur.ini /etc/php/8.x/mods-available/

# Creating a symbolic link for the PHP CLI and FPM targets
$ sudo ln -s /etc/php/8.x/mods-available/zpheur.ini /etc/php/8.x/cli/conf.d/22-zpheur.ini
$ sudo ln -s /etc/php/8.x/mods-available/zpheur.ini /etc/php/8.x/fpm/conf.d/22-zpheur.ini

# If you're running PHP-FPM, you may need to restart php8.x-fpm to load the extension
```
For other PHP installations, the configuration may be different.
## Usage
For usage and documentation, see [Zpheur skeleton application](https://github.com/onecthree/zpheur) for more info.