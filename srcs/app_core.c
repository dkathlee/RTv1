/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dkathlee <dkathlee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/06 10:25:20 by celva             #+#    #+#             */
/*   Updated: 2020/08/10 16:42:33 by dkathlee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rtv1.h"

int		rtv_app_create(t_app *app)
{
	if (vku_instance_create(app) == 0)
		handle_error("Instance creation error\n");
	ft_printf("Instance creation done\n");
	if (vku_window_create(app) == 0)
		handle_error("Window creation error\n");
	ft_printf("Window creation done\n");
	if (vku_get_physical_device(&(app->vulkan)) == 0)
		handle_error("Get physical device error\n");
	ft_printf("Get physical device done: %s\n", app->vulkan.phys_device.dev_prop.deviceName);
	if (vku_create_logical_device(&(app->vulkan)) == 0)
		handle_error("Logical device creation error\n");
	ft_printf("Logical device creation done\n");
	if (vku_swapchain_create(&(app->vulkan)) == 0)
		handle_error("Swapchain creation error\n");
	ft_printf("Swapchain creation done\n");
	if (init_render(&(app->vulkan), &app->r) == 0)
		handle_error("Render initialisation error\n");
	/*if (vku_init_render(app) == 0)
		handle_error("Render initialisation error\n");
	ft_printf("Renderer initialization done\n");
	if (vku_create_buffer(&(app->vulkan)) == 0)
		handle_error("Buffer creation error\n");
	ft_printf("Buffer creation done\n");
	if (vku_record_cmb(&(app->vulkan)) == 0)
		handle_error("Cmb record error\n");
	ft_printf("Cmb record done\n");*/
}

void	rtv_app_destroy(t_vulkan *v)
{
	//vkUnmapMemory(v->device, v->buf.dev_mem);
	//vkFreeMemory(v->device, v->buf.dev_mem, NULL);
	//vkDestroyBuffer(v->device, v->buf.buffer, NULL);
	vkDestroySwapchainKHR(v->device, v->swapchain, NULL);
	vkDestroyDevice(v->device, NULL);
	vkDestroyInstance(v->inst, NULL);
}

t_vec3	rotation_axis1(double angle, t_vec3 axis, t_vec3 p)
{
	t_vec3 res;
	double c;
	double s;

	c = cos(angle);
	s = sin(angle);
	res.x = p.x * (c + axis.x * axis.x * (1 - c)) + p.y * (axis.y * axis.x * (1 - c) + axis.z * s) + p.z * (axis.z * axis.x * (1 - c) - axis.y * s);
	res.y = p.x * (axis.x * axis.y * (1 - c) - axis.z * s) + p.y * (c + axis.y * axis.y * (1 - c)) + p.z * (axis.z * axis.y * (1 - c) + axis.x * s);
	res.z = p.x * (axis.x * axis.z * (1 - c) + axis.y * s) + p.y * (axis.y * axis.z* (1 - c) - axis.x * s) + p.z * (c + axis.z * axis.z * (1 - c));
	return (res);
}

void	rtv_app_run(t_app *app, char *fname)
{
	int	run;
	int j;
	//t_retr r;

	//init_struct(&r, fname);
	run = 1;
	j = 0;
	while (run)
    {
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                run = 0;
            }
			else if (evt.type == SDL_KEYDOWN)
			{
				if (evt.key.keysym.sym == SDLK_UP)
				{
					app->r.camera.position = vec3_add_vec3(app->r.camera.position, vec3_mul_f(app->r.camera.rotation, vec3_mod(app->r.camera.rotation)));
					j = 0;
				}
				if (evt.key.keysym.sym == SDLK_DOWN)
				{
					app->r.camera.position = vec3_sub_vec3(app->r.camera.position, vec3_mul_f(app->r.camera.rotation, vec3_mod(app->r.camera.rotation)));
					j = 0;
				}
				if (evt.key.keysym.sym == SDLK_LEFT)
				{
					app->r.camera.position = vec3_add_vec3(app->r.camera.position, (t_vec3){-0.1, 0, 0});
					j = 0;
				}
				if (evt.key.keysym.sym == SDLK_RIGHT)
				{
					app->r.camera.position = vec3_add_vec3(app->r.camera.position, (t_vec3){0.1, 0, 0});
					j = 0;
				}
				if (evt.key.keysym.sym == SDLK_q)
				{
					t_vec3 prev = app->r.camera.rotation;
					app->r.camera.rotation = rotation_axis1(3.14/10, (t_vec3){0, 1, 0}, app->r.camera.rotation);
					float cos = vec3_scalar(prev, app->r.camera.rotation);
					ft_printf("Cos: %f\n", cos);
					//r.camera.rotation = vec3_add_vec3(r.camera.rotation, (t_vec3){0.1, 0, 0});
					j = 0;
				}
				if (evt.key.keysym.sym == SDLK_e)
				{
					t_vec3 prev = app->r.camera.rotation;
					app->r.camera.rotation = rotation_axis1(-3.14/10, (t_vec3){0, 1, 0}, app->r.camera.rotation);
					float cos = vec3_scalar(prev, app->r.camera.rotation);
					ft_printf("Cos: %f\n", cos);
					//r.camera.rotation = vec3d_add_vec3d(r.camera.rotation, (t_vec3){-0.1, 0, 0});
					j = 0;
				}
			}
        }
		if (j == 0)
		{
			draw_frame1(&(app->vulkan), &(app->r));
			j = 1;
		}
    }
	rtv_app_destroy(&(app->vulkan));
    SDL_Quit();
}
