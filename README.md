# Zpheur
> ⚠️ Used only for non-production (untested and non-stable release)
 
Zpheur is a web framework for PHP written in C delivered as extension. It included router, middleware, dependency injection, dotenv loader and many more. Aim for minimalist project structure without bloat-installed Composer package.

## Installation
> Extension development are in Ubuntu 20.04, for other GNU/Linux distro or platform OS are not tested

The extension only support PHP version from 8.2.1 or greater, and required ```libpcre3``` ```openssl```. For installing the library (Ubuntu 20.04):
```bash
$ sudo apt install libpcre3 -y
$ sudo apt install openssl -y
```
The extension itself need other extension to running each feature (optional):
```bash
# for Zpheur Voile (MongoDB ODM), you need MongoDB extension driver for PHP
$ sudo apt install php8.2-mongodb -y
... or php8.3-mongodb, php8.4-mongodb... depend on you PHP version
```
Clone the repository:
```bash
$ git clone https://github.com/zpheur/czpheur
```
And compile the extension:
```bash
$ cd czpheur
$ phpize
$ ./configure
$ make
$ sudo make install
```
While ```$ sudo make install``` only move the shared object ```zpheur.so``` to PHP extension directory, you may manually install ```zpheur.ini``` to PHP configuration directory. If the PHP installation using ```apt``` package manager via ```ppa:ondrej/php```, usually located in the following directory ```/etc/php/8.x```.\
\
For the installation:
```bash
# Under czpheur clone directory
$ sudo mv zpheur.ini /etc/php/8.x/mods-available/

# Doing a symbolic link for PHP CLI and FPM target
$ sudo ln -s /etc/php/8.x/mods-available/zpheur.ini /etc/php/8.x/cli/conf.d/22-zpheur.ini
$ sudo ln -s /etc/php/8.x/mods-available/zpheur.ini /etc/php/8.x/fpm/conf.d/22-zpheur.ini
```
If you use PHP-FPM, may do restart ```php8.x-fpm``` for load the extension.
## Basic Usage
For usage and documentation, see [Zpheur skeleton application](https://github.com/onecthree/zpheur) for more info.