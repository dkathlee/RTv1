/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raytracing.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/14 14:23:15 by marvin            #+#    #+#             */
/*   Updated: 2020/03/23 16:32:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rtv1.h"

t_vec3	CanvasToViewport(int i, int j, t_retr *r)
{
    t_vec3	res;

    res.x = j * r->vw / WIN_WIDTH;
    res.y = i * r->vh / WIN_HEIGHT;
    res.z = r->d;
    return (res);
}

void	intersectRaySphere(t_vec3 ds, t_vec3 o, t_sphere sphere, double *t1, double *t2)
{
	t_vec3	c;
	double	rad;
	t_vec3 oc;
	double	k1;
	double	k2;
	double	k3;
	double 	discriminant;

	c = sphere.center;
	rad = sphere.radius;
	oc = vec3d_sub_vec3d(o, c);
	k1 = vec3d_scalar(ds, ds);
	k2 = vec3d_scalar(oc, ds) * 2;
	k3 = vec3d_scalar(oc, oc) - rad * rad;
	discriminant = k2 * k2 - 4 * k1 * k3;
	if (discriminant < 0)
	{
		*t1 = DBL_MAX;
		*t2 = DBL_MAX;
	}
	else
	{
		*t1 = (-k2 + sqrt(discriminant)) / (2 * k1);
		*t2 = (-k2 - sqrt(discriminant)) / (2 * k1);
	}
}

t_sphere	closestIntersection(t_retr *r, t_vec3 ds, t_vec3 o, double t_min, double t_max, int *n, double *closest_t)
{
	t_sphere	sphere;
	int			i;
	double		t1;
	double		t2;
	
	*closest_t = DBL_MAX;
	*n = 0;
	sphere = (t_sphere){(t_vec3){0, 0, 0}, 0, (t_vec3){0, 0, 0}};
	i = 0;
	while (i < r->n_fig)
	{
		intersectRaySphere(ds, o, (r->figures)[i], &t1, &t2);
		if ((t_min < t1 && t_max > t1) && t1 < *closest_t)
		{
			*closest_t = t1;
			sphere = (r->figures)[i];
			(*n)++;
		}
		if ((t_min < t2 && t_max > t2) && t2 < *closest_t)
		{
			*closest_t = t2;
			sphere = (r->figures)[i];
			(*n)++;
		}
		i++;
	}
	return (sphere);
}

double	computeLighting(t_vec3 p, t_vec3 n, t_retr *r, int s)
{
	double		res;
	int			i;
	t_vec3		l;
	t_vec3		v;
	double		n_scal_l;
	double		shadow_t;
	t_sphere	shadow_sphere;
	int			n_sphere;
	double		t_max;

	v = vec3d_mul_d(r->ds, -1.0);
	res = 0.0;
	i = 0;
	while (i < r->n_lig)
	{
		if ((r->lights)[i].type == 'a')
			res += (r->lights)[i].intensity;
		else 
		{
			if ((r->lights)[i].type == 'p')
			{
				l = vec3d_sub_vec3d((r->lights)[i].position, p);
				t_max = 1;
			}
			else
			{
				l = (r->lights)[i].direction;
				t_max = DBL_MAX;
			}
			
			//Проверка тени
			shadow_sphere = closestIntersection(r, l, p, 0.001, t_max, &n_sphere, &shadow_t);
			if (n_sphere != 0)
			{
				i++;
				continue;
			}
			
			//Диффузность
			n_scal_l = vec3d_scalar(n, l);
			if (n_scal_l > 0)
				res += (r->lights)[i].intensity * n_scal_l / (vec3d_mod(n) * vec3d_mod(l));

			//Зеркальность
			if (s != -1)
			{
				t_vec3 r_v = vec3d_sub_vec3d(vec3d_mul_d(n, (vec3d_scalar(n, l) * 2.0)), l);
				double rv_scal_v = vec3d_scalar(r_v, v);
				if (rv_scal_v > 0)
					res += (r->lights)[i].intensity * pow(rv_scal_v / (vec3d_mod(r_v) * vec3d_mod(v)), s);
			}
		}
		i++;
	}
	if (res >= 1)
		res = 1.0;
	return (res);
}

t_vec3	traceRay(t_retr *r, t_vec3 ds, t_vec3 o, double t_min, double t_max)
{
    double		closest_t;
	int			n;
	int			i;
	t_sphere	sphere;

	sphere = closestIntersection(r, ds, o, t_min, t_max, &n, &closest_t);
	if (n == 0)
		return ((t_vec3){255, 255, 255});
	
	
	t_vec3 p = vec3d_add_vec3d(vec3d_mul_d(ds, closest_t), o);
	t_vec3 norm = vec3d_sub_vec3d(p, sphere.center);
	norm = vec3d_mul_d(norm, (1 / vec3d_mod(norm)));
	
	return (vec3d_mul_d(sphere.color, computeLighting(p, norm, r, sphere.specular)));
}

int		color_int(t_vec3 c)
{
	int res;

	res = 0 << 24 | (int)(c.x) << 16 | (int)(c.y) << 8 | (int)(c.z);
	return (res);
}

void    raytracing(t_retr *r, t_app *app)
{
    int i;
    int j;
	t_vec3 color;

    //не забыть задавать начальные значения в другом месте
	r->n_fig = 4;
	r->n_lig = 3;
	r->figures = (t_sphere*)malloc(sizeof(t_sphere) * r->n_fig);
	r->lights = (t_light*)malloc(sizeof(t_light) * r->n_lig);
    r->vw = 1.5;
    r->vh = 1;
    r->d = 1;
    (r->figures)[0] = (t_sphere){(t_vec3){0, -1.0, 3.0}, 1.0, (t_vec3){255, 0, 0}, 500};
    (r->figures)[1] = (t_sphere){(t_vec3){2.0, 0, 4.0}, 1.0, (t_vec3){0, 0, 255}, 500};
    (r->figures)[2] = (t_sphere){(t_vec3){-2.0, 0, 4.0}, 1.0, (t_vec3){0, 255, 0}, 10};
	(r->figures)[3] = (t_sphere){(t_vec3){0, -5001.0, 0}, 5000, (t_vec3){255, 255, 0}, 1000};
	(r->lights)[0] = (t_light){'a', 0.2, (t_vec3){0, 0, 0}, (t_vec3){0, 0, 0}};
	(r->lights)[1] = (t_light){'p', 0.6, (t_vec3){2, 1, 0}, (t_vec3){0, 0, 0}};
	(r->lights)[2] = (t_light){'d', 0.2, (t_vec3){0, 0, 0}, (t_vec3){1, 4, 4}};
    r->o = (t_vec3){0, 0, 0};

    i = 0;
    while (i < WIN_HEIGHT)
    {
        j = 0;
        while (j < WIN_WIDTH)
        {
            r->ds = CanvasToViewport(WIN_HEIGHT/2 - i, j - WIN_WIDTH/2, r);
			color = traceRay(r, r->ds, r->o, 1.0, DBL_MAX);
			int col = color_int(color);
			((int*)(app->vulkan.buf.mem_ptr))[i * WIN_WIDTH + j] = col;
            j++;
        }
        i++;
    }
}
