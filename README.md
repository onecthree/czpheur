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

## Basic Usage
Install project template using composer:
```bash
$ composer require zpheur/zpheur
```