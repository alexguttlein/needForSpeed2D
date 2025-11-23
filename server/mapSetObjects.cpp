#include "mapSetObjects.h"

void MapSetObjects::createStaticBody(b2WorldId world, const b2Vec2& position, const std::vector<b2Polygon>& fixtures) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody;
    bodyDef.position = position;

    b2BodyId body = b2CreateBody(world, &bodyDef);
    
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    
    for (const auto& shape : fixtures) {
        // La b2Polygon ya contiene el hull calculado. 
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
            // --- 1. OBJETO RECTÁNGULO (BOX) ---
            
            // 1.1. Calcular dimensiones en metros (half-widths)
            float halfWidthMeters = obj.width * 0.5f;
            float halfHeightMeters = obj.height * 0.5f;
            
            // 1.2. Crear el Box y asignar el resultado a 'poly'
            // NOTA: b2MakeBox devuelve la forma por valor.
            b2Polygon poly = b2MakeBox(halfWidthMeters, halfHeightMeters); 

            fixtures.push_back(poly);

            b2Vec2 pos;
            pos.x = (obj.x + obj.width * 0.5f);
            pos.y = (obj.y + obj.height * 0.5f);
            // // 1.3. Calcular la posición CENTRAL en metros
            // b2Vec2 pos(
            //     (obj.x + obj.width * 0.5f) * Constants::SCALE_METER_TO_PIXEL,
            //     (obj.y + obj.height * 0.5f) * Constants::SCALE_METER_TO_PIXEL
            // );

            createStaticBody(world, pos, fixtures);
        }
        else {
            // --- 2. OBJETO POLÍGONO ---

            if (obj.polygonPoints.size() < 3) {
                std::cerr << "Advertencia: Polígono ignorado, tiene menos de 3 vértices." << std::endl;
                continue;
            }
            if (obj.polygonPoints.size() > 8) {
                std::cerr << "Advertencia: Polígono ignorado, Box2D C API solo soporta 8 vértices convexos." << std::endl;
                continue;
            }

            // 2.1. Convertir vértices de píxeles a metros
            std::vector<b2Vec2> scaledVertices;
            scaledVertices.reserve(obj.polygonPoints.size());
            for (const auto& point : obj.polygonPoints) {
                scaledVertices.push_back({
                    point.x , 
                    point.y 
                });
            }

            // 2.2. Calcular el Convex Hull (OBLIGATORIO para b2MakePolygon en la API C)
            b2Hull hull = b2ComputeHull(scaledVertices.data(), (int)scaledVertices.size());
            
            if (hull.count > 0) {
                // 2.3. Crear el Polígono Box2D
                b2Polygon poly = b2MakePolygon(&hull, 0.0f); // 0.0f = radio
                fixtures.push_back(poly);

                // 2.4. La posición del cuerpo es el origen del objeto (en metros)
                // Usamos el origen del objeto, ya que b2ComputeHull y b2MakePolygon 
                // ya manejan la geometría del polígono.
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