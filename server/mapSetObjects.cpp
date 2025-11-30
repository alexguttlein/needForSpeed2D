#include "mapSetObjects.h"

void MapSetObjects::createStaticBody(b2WorldId world, const b2Vec2& position, const std::vector<b2Polygon>& fixtures) {
    
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;
    b2BodyId body = b2CreateBody(world, &bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    
    for (const auto& shape : fixtures) {
        b2CreatePolygonShape(body, &shapeDef, &shape);
        //b2Shape_SetFriction(shapeId, 0.5f);
    }
}


void MapSetObjects::createBodiesFromObjects(
    b2WorldId world,
    const std::vector<MapObject>& objects)
{
    std::cout << "[Box2D] Creando " << objects.size() << " cuerpos estáticos..." << std::endl;

    for (const auto& obj : objects) {
        
        std::vector<b2Polygon> fixtures;

        if (!obj.isPolygon) {

            float halfWidthMeters = obj.width * 0.5f;
            float halfHeightMeters = obj.height * 0.5f;
            b2Polygon poly = b2MakeBox(halfWidthMeters, halfHeightMeters); 
            fixtures.push_back(poly);

            b2Vec2 pos;
            pos.x = (obj.x + obj.width * 0.5f);
            pos.y = (obj.y + obj.height * 0.5f);
            createStaticBody(world, pos, fixtures);
        }
        else {
            if (obj.polygonPoints.size() < 3) {
                std::cerr << "Advertencia: Polígono ignorado, tiene menos de 3 vértices." << std::endl;
                continue;
            }
            if (obj.polygonPoints.size() > 8) {
                std::cerr << "Advertencia: Polígono ignorado, Box2D C API solo soporta 8 vértices convexos." << std::endl;
                continue;
            }
            std::vector<b2Vec2> scaledVertices;
            scaledVertices.reserve(obj.polygonPoints.size());
            for (const auto& point : obj.polygonPoints) {
                scaledVertices.push_back({
                    point.x , 
                    point.y 
                });
            }

            b2Hull hull = b2ComputeHull(scaledVertices.data(), (int)scaledVertices.size());
            
            if (hull.count > 0) {
                b2Polygon poly = b2MakePolygon(&hull, 0.0f); // 0.0f = radio
                fixtures.push_back(poly);

                b2Vec2 pos;
                pos.x = obj.x ;
                pos.y = obj.y ;
                
                createStaticBody(world, pos, fixtures);
            } else {
                 std::cerr << "Advertencia: Falló el cálculo del Convex Hull. Polígono ignorado." << std::endl;
            }
        }
    }
    std::cout << "[Box2D] Creación de cuerpos estáticos finalizada." << std::endl;
}
