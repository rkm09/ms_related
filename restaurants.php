<?php
/**
 * Created by PhpStorm.
 * User: truth
 * Date: 15/02/16
 * Time: 15:39
 */
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Validator\Constraints As Assert;

$app;

$api = $app['controllers_factory'];

$api->get('/restaurants',function() use($app){
    $sql = 'SELECT * FROM restaurants';
    $result = $app['db']->fetchAll($sql);
    return $app->json($result);
});

$api->get('/restaurants/{id}',function($id) use($app){
    $sql = 'SELECT * FROM restaurants WHERE id = ?';
    $result = $app['db']->fetchAssoc($sql,[$id]) ;
    return $app->json($result);
});

$api->before(
    function (Request $request) {
        if (0 === strpos($request->headers->get('Content-Type'),
                'application/json')) {
            $data = json_decode($request->getContent(), true);
            $request->request->replace(is_array($data) ? $data : array());
        }
    }
);

$api->post('/restaurants',function(Request $request) use($app){
    $user =  $app['user'];
    if($user === null)
        return new Response(null,401);
    else {
        $data = $request->request->all();
        $restaurantValidator = new Assert\Collection([
            'review' => [new Assert\NotBlank(), new Assert\Length(['max' => 255])],
            'rating' => [new Assert\NotBlank(), new Assert\Length(['max' => 255])]
        ]);
        $errors = $app['validator']->validateValue($data, $restaurantValidator);
        if (count($errors) > 0) {
            $errorList = [];
            /** @var Symfony\Component\Validator\ConstraintViolation £error */
            foreach ($errors as $error) {
                $errorList[$error->getPropertyPath()] = $error->getMessage();
            }
            return $app->json($errorList, 400);
        } else {
            $app['db']->insert('reviews',
                ['review' => $data['review'], 'rating' => $data['rating'],'rating' => $data['rating']]);
            $id = $app['db']->lastInsertId();
            return new Response(null, 201, ['Location' => '/api/restaurants/' . $id]);
        }
    }
});


$api->delete('restaurants/{id}',function(Request $request,$id) use($app){
    $user =  $app['user'];
    if($user === null)
        return new Response(null,401);
    else {
        $result = $app['db']->delete('restaurants', ['id' => $id]);
        return new Response(null, 204);
    }
});

$api->put('/restaurants/',function(Request $request) use($app){
    $user =  $app['user'];
    if($user === null)
        return new Response(null,401);
    else {
        $data = $request->request->all();
        $app['db']->update('reviews', ['review' => $data['review']], ['userid' => $data['id']]);
        return new Response(null, 204);
    }
});

$api->post('/users',function(Request $request) use($app){
    $userManager = $app['user.manager'];
    $user = $userManager->createUser($request->request->get('email'),$request->request->get('password'));
    $userManager->insert($user);
    return new Response(null,201);
});

return $api;