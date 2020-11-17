#include "random.h"

std::default_random_engine & global_urng( )
{
    static std::default_random_engine u{};
    return u;
}
void randomize( )
{
    static std::random_device rd{};
    global_urng().seed( rd() );
}
//[min, max]
int numAleatorio(int minimo, int maximo)
{
    /*
    -std=c++11 minimo requerido en las propiedades del compilador
    DevCpp: Opciones de Proyecto (Ctrl+H) -> Argumentos para el programa -> C++ compiler, añadir -std=c++11
    */


    static std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d( global_urng(), parm_t{minimo, maximo} );

}
double numAleatorioReal(double minimo, double maximo)
{
    static std::uniform_real_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d( global_urng(), parm_t{minimo, maximo} );
}
