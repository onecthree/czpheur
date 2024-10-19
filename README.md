# Zpheur
> ⚠️ Only testing for non-production or personal web (non-stable release)
 
Zpheur is a web framework for PHP written in C delivered as extension. It included router, middleware, dependency injection, dotenv loader and many more. Aim for minimalist project structure without bloat-installed third-party package.
```php
<?php declare( strict_types = 1 );
namespace App\Http\Action;

use App\Service\Http\Message\Response;
use App\Http\Abstract\BaseAction;
use App\Http\Middleware\Welcome;


#[Middleware(dest: Welcome::class)]
class IndexAction extends BaseAction
{   
    #[Route\GET(dest: '/')]
    public function getIndex( Response $response ): Response
    {   
        return $response->view('welcome');
    }
}
```

## Installation
> Extension development are in Ubuntu 20.04, for other GNU/Linux distro or platform OS are not tested

The extension only support PHP version from 8.2.1 or greater, and required ```libpcre3-dev``` ```openssl```. For installing the library (Ubuntu 20.04):
```bash
$ sudo apt install libpcre3-dev -y
$ sudo apt install openssl -y
```
Install ```php-mongodb``` (required):
```bash
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
Install project template using composer:
```bash
$ composer require zpheur/zpheur
```