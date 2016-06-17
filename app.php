<?php

/**
 * @var Silex\Application $app
 */
$app = require_once __DIR__ . '/bootstrap.php';

$app->get('/hello/world',function() {
    return 'hello ' ;
});

$app->get('/hello/world/{name}',function($name) use ($app){
    return 'Hello '. $app->escape($name) .'<br> How are you today ? :)';
});

$app->get('/hello/date/{date2}', function($date2) use($app){
    $s = 'The day of the week is : ';
    $date1 = $app->escape($date2) ;
    $fin = new DateTime($date1);
    return $s.$fin->format('l');
});


//$app->mount('/v1',include __DIR__.'/restaurants.php');

return $app;

