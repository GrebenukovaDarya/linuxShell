/*node browser: true */ /*global $ */
$(document).ready(function () {
    $(".slider").slick({
        dots: true,
        infinite: true,
        responsive: [{
            breakpoint: 900,
            settings: {
                dots: true,
                infinite: true,
                slidesToScroll: 1,
                slidesToShow: 1
            }
        }],
        slidesToScroll: 1,
        slidesToShow: 3
    });
});
